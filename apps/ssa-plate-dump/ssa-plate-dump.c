/*
 * ssa-plate-dump.c
 *
 *  Created on: Feb 15, 2013
 *      Author: amyznikov
 */


#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS     64  /* See man fseeko */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <limits.h>
#include "ssa-detection.h"
#include "ccarray.h"

/** Supported file compression types */
typedef
enum compression_t {
  compression_unknown = -1,
  compression_none,
  compression_bzip,
  compression_gzip
} compression_t;

typedef
struct sbox_s {
  double ramin, decmin;
  double ramax, decmax;
} sbox_s;


/**
 * Image quality bits,
 * see http://surveys.roe.ac.uk/ssa/dboverview.html
 */
typedef
enum QualityFlags {
  QF_OCF  =    1,     /*<  Orientation calculation failed       0     1  Information   Image perfectly round */
  QF_ECF  =    2,     /*<  Ellipticity calculation failed       1     2  Information   Image perfectly straight */
  QF_TMD  =    4,     /*<  Image too multiple for deblending    2     4  Warning   Image split into too many fragments */
  QF_BI   =   16,     /*<  Bright image                         4    16  Information   Image has pixels brighter than highest areal profile level */
  QF_LI   =   64,     /*<  Large image                          6    64  Warning   Image has area greater than maximum specified for deblending */
  QF_PBI  =  128,     /*<  Possible bad image                   7   128  Warning   Image is in step-wedge/label region */
  QF_NVBS = 1024,     /*<  Image near very bright star         10  1024  Warning   Image may be spurious due to bright star artifact */
  QF_HA   = 2048,     /*<  Halo artifact                       11  2048  Strong warning  Image is likely spurious result of bright star halo */
  QF_DSA  = 8192,     /*<  Diffraction spike artifact          13  8192  Strong warning  Image is likely spurious result of a bright star diffraction spike */
  QF_TA   =16384,     /*<  Track artifact                      14 16384  Strong warning  Image is likely spurious result of satellite/'plane/scratch track */
  QF_ITB  =65536,     /*<  Image touches boundary              16 65536  Severe defect   Image pixels partially missing */
} QualityFlags;


/**
 * Output control bits
 */
enum {
  OUTPUT_RECNUM = 1,        /*< true to print record index into output */
  OUTPUT_HEADER = 2,        /*< true to print columns header line */
  OUTPUT_FJUNK  = 4,        /*< true if junk filter is requested */
  OUTPUT_DROP_PARENTS = 8,  /*< true to drop parents of deblends */
  OUTPUT_VERBOSE = 16,      /*< true to print some progress info */
  OUTPUT_BINARY = 32,       /*< true to write binary output file */
  OUTPUT_SBOX   = 64,       /*< true to use sbox selection */
  OUTPUT_DEG = 128,         /*< true to write RA/DEC columns in degrees */
};


/** show usage info. */
static void show_usage( FILE * output )
{
  fprintf(output,"Dump SuperCOSMOS binary plate file to stdout as text\n");
  fprintf(output,"USAGE:\n");
  fprintf(output,"   ssa-plate-dump [OPTIONS] [FILE] [-o OUTPUT-FILE-NAME]\n");
  fprintf(output,"\n");
  fprintf(output,"OPTIONS:\n");
  fprintf(output,"   -j  treat input file as compressed by bzip2\n");
  fprintf(output,"   -z  treat input file as compressed by gzip\n");
  fprintf(output,"   -v  print some diagnostics to stderr\n");
  fprintf(output,"   capacity=size_t  set internal array capacity\n");
  fprintf(output,"\n");
  fprintf(output,"OUTPUT CONTROL:\n");
  fprintf(output,"   -h  include columns header\n");
  fprintf(output,"   -r  include (one-based) record index\n");
  fprintf(output,"   -c  drop parents of deblends\n");
  fprintf(output,"   -b  output in SuperCOSMOS binary format (useful to cleanup plate from junk and parents)\n");
  fprintf(output,"   -f  apply junk filter\n");
  fprintf(output,"   -o  set output file name from next argument\n");
  fprintf(output,"   -u  output RA/DEC in units specified in next argument {deg,rad} \n");
  fprintf(output,"   minmag=float  minimal (bright) output magnitude\n");
  fprintf(output,"   maxmag=float  maximal (faint) output magnitude\n");
  fprintf(output,"   sbox=ramin,decmin,ramax,decmax (in radians)\n");
  fprintf(output,"\n");
  fprintf(output,"If no input file is given then read plate file from stdin (to allow piped processing)\n");
  fprintf(output,"\n");
  fprintf(output,"Examples:\n");
  fprintf(output,"  ssa-plate-dump -h 1-65537.dat\n");
  fprintf(output,"  ssa-plate-dump -cbf 1-65537.dat -u deg -o 1-65537.clean.dat\n");
}


/** uses fopen() for uncompressed streams, and popen() for compressed ones */
static FILE * open_file( const char * fname, compression_t * compression )
{
  char cmd[PATH_MAX] = {0};
  FILE * input = NULL;

  if ( *compression == compression_unknown )
  {
    const char * suffix;

    if ( (suffix = strstr(fname, ".bz2")) && *(suffix + 4) == 0 ) {
      *compression = compression_bzip;
    }
    else if ( (suffix = strstr(fname, ".bz")) && *(suffix + 3) == 0 ) {
      *compression = compression_bzip;
    }
    else if ( (suffix = strstr(fname, ".gz")) && *(suffix + 3) == 0 ) {
      *compression = compression_gzip;
    }
    else {
      *compression = compression_none;
    }
  }

  switch ( *compression )
  {
  case compression_bzip:
    snprintf(cmd, sizeof(cmd) - 1, "bzip2 -dc '%s'", fname);
    if ( !(input = popen(cmd, "r"))) {
      fprintf(stderr, "popen('%s') fails: %s\n", cmd, strerror(errno));
    }
    break;

  case compression_gzip:
    snprintf(cmd, sizeof(cmd) - 1, "gzip -dc '%s'", fname);
    if ( !(input = popen(cmd, "r"))) {
      fprintf(stderr, "popen('%s') fails: %s\n", cmd, strerror(errno));
    }
    break;

  case compression_none:
    if ( !(input = fopen(fname, "r"))) {
      fprintf(stderr, "fopen('%s') fails: %s\n", fname, strerror(errno));
    }
    break;

  default:
    fprintf(stderr, "BUG IN CODE: invalid compression tag=%d\n", *compression);
    break;
  }

  return input;
}

/** uses fclose() for uncompressed streams, and pclose() for compressed ones */
static void close_file( FILE * input, compression_t compression )
{
  if ( input && input != stdin ) {
    if ( compression > compression_none ) {
      pclose(input);
    }
    else {
      fclose(input);
    }
  }
}

/** load objects into array from input stream */
static int load_objects(FILE * input, ccarray_t * objects)
{
  size_t size;

  while ( (size = ccarray_size(objects)) < ccarray_capacity(objects) )
  {
    ssa_detection2 * obj = ccarray_peek_end(objects);
    if ( fread(obj, sizeof(*obj), 1, input) != 1 ) {
      break;
    }
    ccarray_set_size(objects, size + 1);
  }

  return feof(input) ? 0 : -1;
}


/** compare pair of objects using objID as key */
static int cmp_objid( const void * p1, const void * p2 )
{
  const ssa_detection2 * obj1 = p1;
  const ssa_detection2 * obj2 = p2;

  if ( obj1->objID < obj2->objID ) {
    return -1;
  }
  if ( obj1->objID > obj2->objID ) {
    return +1;
  }
  return 0;
}

static int sbox_hittest( const sbox_s * sbox, double ra, double dec )
{
  return ra >= sbox->ramin && ra <= sbox->ramax && dec >= sbox->decmin && dec <= sbox->decmax;
}


static ssa_detection2 * find_parent( const ssa_detection2 * obj, const ccarray_t * objects )
{
  if ( obj->parentID != obj->objID ) {
    const size_t size = ccarray_size(objects);
    const size_t pos = ccarray_lowerbound(objects, 0, size, cmp_objid, &obj->parentID);
    if ( pos < size ) {
      return ccarray_peek(objects, pos);
    }
  }
  return NULL;
}

/** Sophisticated junk tester. The 'objects' array MUST be sorted using cmp_objid() comparator */
static int isjunk(const ssa_detection2 * obj, const ccarray_t * objects)
{
  const ssa_detection2 * parent;

  /* Image is definitely invalid or image pixels partially missing: */
  if ( (obj->quality & (QF_ITB | QF_ECF)) ) {
    return 1;
  }

  /* too faint image */
  if ( obj->ap3 < 3 || obj->cosmag > -19.5 ) {
    return 1;
  }

  /* Failt track artifact but not a very brigt star */
  if ( obj->class == 1  )
  {
    if ( (obj->quality & QF_TA) && !(obj->quality & QF_BI) && obj->ap7 == 0 ) {
      return 1;
    }

    /* Track artifact */
    if ( ((obj->quality & (QF_TA | QF_NVBS)) == (QF_TA | QF_NVBS)) ) {
      return 1;
    }

    /* Track artifact */
    if ( ((obj->quality & (QF_BI | QF_NVBS)) == (QF_BI | QF_NVBS)) ) {
      if ( (parent = find_parent(obj, objects)) && ( parent->cosmag < -27 ) ) {
        if ( hypot(parent->xCen - obj->xCen, parent->yCen - obj->yCen) < 650) {
          return 1;
        }
      }
    }
  }

  /* faint halo artifacts */
  if ( (obj->quality & (QF_HA | QF_DSA)) && obj->ap5 == 0 ) {
    return 1;
  }


  /* bright spike artifacts */
  if ( (obj->quality & (QF_DSA | QF_NVBS)) == (QF_DSA | QF_NVBS) ) {
    return 1;
  }


  /* childs of bright parents */
  if ( ((obj->quality & (QF_NVBS | QF_HA)) == (QF_NVBS | QF_HA)) && (obj->ap4 == 0) ) {
    return 1;
  }

  if ( (obj->blend > 0) && (parent = find_parent(obj, objects)) )
  {
    if ( parent->cosmag < -27 )
    {
      if ( (obj->quality & (QF_NVBS | QF_HA)) && (obj->ap7 == 0) ) {
        return 1;
      }

      if ( (obj->ap8 == 0 && hypot(parent->xCen - obj->xCen, parent->yCen - obj->yCen) < 650) ) {
        return 1;
      }
    }
  }


  return 0;
}

/** print header line if acceptable */
static int dump_header_line( FILE * output, int output_opts )
{
  if ( (output_opts & OUTPUT_HEADER) && !(output_opts & OUTPUT_BINARY) )
  {
    if ( output_opts & OUTPUT_RECNUM ) {
      fprintf(output, "recnum\t");
    }

    fprintf(output,
        "objID\t"
        "parentID\t"
        "ra\t"
        "dec\t"
        "xmin\t"
        "xmax\t"
        "ymin\t"
        "ymax\t"
        "area\t"
        "ipeak\t"
        "cosmag\t"
        "isky\t"
        "xCen\t"
        "yCen\t"
        "aU\t"
        "bU\t"
        "thetaU\t"
        "aI\t"
        "bI\t"
        "thetaI\t"
        "class\t"
        "pa\t"
        "ap1\t"
        "ap2\t"
        "ap3\t"
        "ap4\t"
        "ap5\t"
        "ap6\t"
        "ap7\t"
        "ap8\t"
        "blend\t"
        "quality\t"
        "prfStat\t"
        "prfMag\t"
        "gMag\t"
        "sMag\n"
        );
  }

  return 0;
}

/** print object in requested format */
static int dump_object( FILE * output, int output_opts, const ssa_detection2 * obj, size_t recnum )
{
  if ( output_opts & OUTPUT_BINARY )
  {
    if ( fwrite(obj, sizeof(*obj), 1, output) != 1 ) {
      fprintf(stderr,"Can't write output: %d (%s)\n", errno, strerror(errno));
      return 1;
    }
  }
  else
  {
    if ( (output_opts & OUTPUT_RECNUM) ) {
      fprintf(output, "%16zu\t", recnum);
    }

    int n = fprintf(output,
      "%16"PRId64"\t"
      "%16"PRId64"\t"
      "%16.9f\t"
      "%+16.9f\t"
      "%16.2f\t"
      "%16.2f\t"
      "%16.2f\t"
      "%16.2f\t"
      "%9d\t"
      "%9.1f\t"
      "%9.3f\t"
      "%9.1f\t"
      "%12.2f\t"
      "%12.2f\t"
      "%9.3f\t"
      "%9.3f\t"
      "%6d\t"
      "%9.3f\t"
      "%9.3f\t"
      "%6d\t"
      "%3u\t"
      "%6d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9d\t"
      "%9.3f\t"
      "%9.3f\t"
      "%9.3f\t"
      "%9.3f\n",
      obj->objID,
      obj->parentID,
      obj->ra,
      obj->dec,
      obj->xmin,
      obj->xmax,
      obj->ymin,
      obj->ymax,
      obj->area,
      obj->ipeak,
      obj->cosmag,
      obj->isky,
      obj->xCen,
      obj->yCen,
      obj->aU,
      obj->bU,
      obj->thetaU,
      obj->aI,
      obj->bI,
      obj->thetaI,
      obj->class,
      obj->pa,
      obj->ap1,
      obj->ap2,
      obj->ap3,
      obj->ap4,
      obj->ap5,
      obj->ap6,
      obj->ap7,
      obj->ap8,
      obj->blend,
      obj->quality,
      obj->prfStat,
      obj->prfMag,
      obj->gMag,
      obj->sMag
    );

    if ( n <= 0 ) {
      fprintf(stderr,"Can't write output: %d (%s)\n", errno, strerror(errno));
      return 1;
    }
  }

  return 0;
}


/** main() */
int main(int argc, char *argv[])
{
  const char * inputfilename = NULL;
  const char * outputfilename = NULL;

  FILE * input = stdin;
  FILE * output = stdout;

  compression_t compression = compression_unknown;
  ccarray_t * objects = NULL;
  size_t capacity = 15000000;

  int output_opts = 0;

  double minmag = -10;
  double maxmag = +30;
  size_t i, size;

  sbox_s sbox =
    { 0, 0, 0, 0 };


  /* parse command line */
  for ( i = 1; i < (size_t)argc; ++i )
  {
    if ( strcmp(argv[i],"--help") == 0 ) {
      show_usage(stdout);
      return 0;
    }

    if ( strncmp(argv[i],"minmag=",7) == 0 )
    {
      if ( sscanf(argv[i]+7,"%lf", &minmag) != 1 ) {
        fprintf(stderr,"Invalid value %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i],"maxmag=",7) == 0 )
    {
      if ( sscanf(argv[i]+7,"%lf", &maxmag) != 1 ) {
        fprintf(stderr,"Invalid value %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i],"sbox=",5) == 0 )
    {
      if ( sscanf(argv[i] + 5, "%lf,%lf,%lf,%lf", &sbox.ramin, &sbox.decmin, &sbox.ramax, &sbox.decmax) != 4 ) {
        fprintf(stderr,"Invalid value in sbox definition %s\n", argv[i]);
        return 1;
      }

      output_opts |= OUTPUT_SBOX;

    }
    else if ( strncmp(argv[i],"sboxd=",6) == 0 )
    {
      if ( sscanf(argv[i] + 6, "%lf,%lf,%lf,%lf", &sbox.ramin, &sbox.decmin, &sbox.ramax, &sbox.decmax) != 4 ) {
        fprintf(stderr,"Invalid value in sbox definition %s\n", argv[i]);
        return 1;
      }

      sbox.ramin  *= M_PI / 180;
      sbox.decmin *= M_PI / 180;
      sbox.ramax  *= M_PI / 180;
      sbox.decmax *= M_PI / 180;
      output_opts |= OUTPUT_SBOX;

    }
    else if ( strncmp(argv[i],"capacity=",9) == 0 ) {
      if ( sscanf(argv[i] + 9, "%zu", &capacity) != 1 ) {
        fprintf(stderr, "invalid argument value %s\n", argv[i]);
        return 1;
      }
    }
    else if ( *argv[i] == '-' )
    {
      const char * opt = argv[i] + 1;
      if ( *opt == 'o' )
      {
        if ( (int)++i >= argc ) {
          fprintf(stderr, "ERROR: output file name expected after '-o' command line switch\n");
          return 1;
        }

        outputfilename = argv[i];
        continue;
      }

      if ( *opt == 'u' )
      {
        if ( (int)++i >= argc ) {
          fprintf(stderr, "ERROR: No unit name provided after '-u' switch\n");
          return 1;
        }

        if ( strncmp(argv[i],"rad", 3) == 0 ) {
          output_opts &= ~OUTPUT_DEG;
        }
        else if ( strncmp(argv[i],"deg", 3) == 0 ) {
            output_opts |= OUTPUT_DEG;
        }
        else {
          fprintf(stderr, "ERROR: Invalid unit name after '-u' switch. One of rad or deg is expected\n");
          return 1;
        }

        continue;
      }

      while ( *opt )
      {
        switch ( *opt++ )
        {
        case 'j':
          if ( compression > compression_none ) {
            fprintf(stderr,"error: multiple compression options: '%c' in %s\n", *opt, argv[i]);
            return 1;
          }
          compression = compression_bzip;
          break;
        case 'z':
          if ( compression > compression_none ) {
            fprintf(stderr,"error: multiple compression options: '%c' in %s\n", *opt, argv[i]);
            return 1;
          }
          compression = compression_gzip;
          break;
        case 'h':
          output_opts |= OUTPUT_HEADER;
          break;
        case 'r':
          output_opts |= OUTPUT_RECNUM;
          break;
        case 'c':
          output_opts |= OUTPUT_DROP_PARENTS ;
          break;
        case 'b':
          output_opts |= OUTPUT_BINARY;
          break;
        case 'f':
          output_opts |= OUTPUT_FJUNK;
          break;
        case 'v':
          output_opts |= OUTPUT_VERBOSE;
          break;
        default:
          fprintf(stderr,"Error: invalid option '%c' in %s\n", *opt, argv[i] );
          return 1;
        }
      }
    }
    else if ( inputfilename == NULL ) {
      inputfilename = argv[i];
    }
    else {
      fprintf(stderr, "Too many input file names (only one allowed)\n");
      show_usage(stderr);
      return 1;
    }
  }

  if ( compression > compression_none && !inputfilename ) {
    fprintf(stderr,"error: decompression from stdin is not supported\n");
    return 1;
  }

  /* open input file if requested */
  if ( inputfilename && !(input = open_file(inputfilename, &compression)) ) {
    fprintf(stderr,"open_file('%s') fails\n", inputfilename);
    return 1;
  }

  if ( outputfilename && !(output = fopen(outputfilename, "wb")) ) {
    fprintf(stderr, "Can't create '%s': %d (%s)\n", outputfilename, errno, strerror(errno));
    return 1;
  }



  /* Use fast read/write loop if no junk filter is requested */
  if ( !(output_opts & (OUTPUT_FJUNK)) )
  {
    ssa_detection2 obj;

    /* print header line */
    dump_header_line( output, output_opts );

    /* print objects */
    while ( fread(&obj, sizeof(obj), 1, input) == 1 )
    {
      if ( (output_opts & OUTPUT_DROP_PARENTS) && obj.blend < 0 ) {
        continue;
      }

      if ( obj.sMag < minmag || obj.sMag > maxmag ) {
        continue;
      }

      if ( (output_opts & OUTPUT_SBOX) && !sbox_hittest(&sbox, obj.ra, obj.dec) ) {
        continue;
      }

      if ( output_opts & OUTPUT_DEG ) {
        obj.ra *= 180 / M_PI;
        obj.dec *= 180 / M_PI;
      }

      if ( dump_object(output, output_opts, &obj, i) != 0 ) {
        fprintf(stderr,"dump_object() fails\n");
        return 1;
      }
    }

    /* close input stream */
    close_file( input, compression );
  }
  else
  {
    /* apply sophisticated junk filter (slow) */


    /* allocate memory storage */
    if ( !(objects = ccarray_create(capacity, sizeof(ssa_detection2))) ) {
      fprintf(stderr, "ccarray_create(capacity=%zu) fails: %d (%s)\n", capacity, errno, strerror(errno));
      return 1;
    }

    /* load objects */
    if ( load_objects(input, objects) != 0 ) {
      if ( ccarray_size(objects) == capacity && !feof(input) ) {
        fprintf(stderr, "load_objects() fails: Too many objects. Try increase capacity (currently %zu)\n", capacity);
      }
      else {
        fprintf(stderr, "load_objects() fails: %d (%s)\n", errno, strerror(errno));
      }
      return 1;
    }

    /* close input stream */
    close_file( input, compression );


    /* print some diagnostic */
    if ( output_opts & OUTPUT_VERBOSE ) {
      fprintf(stderr, "%zu objects is loaded\n", ccarray_size(objects));
    }


    /* sort by objid if junk filter requested */
    if ( output_opts & OUTPUT_FJUNK ) {
      ccarray_sort(objects, 0, ccarray_size(objects), cmp_objid);
    }



    /* print header line */
    dump_header_line( output, output_opts );


    /* process the list */
    size = ccarray_size(objects);
    for ( i = 0; i < size; ++i )
    {
      ssa_detection2 * obj = ccarray_peek(objects, i);

      if ( (output_opts & OUTPUT_SBOX) && !sbox_hittest(&sbox, obj->ra, obj->dec) ) {
        continue;
      }

      if ( (output_opts & OUTPUT_FJUNK) && isjunk(obj, objects) ) {
        continue;
      }

      if ( (output_opts & OUTPUT_DROP_PARENTS) && obj->blend < 0 ) {
        continue;
      }

      if ( obj->sMag < minmag || obj->sMag > maxmag ) {
        continue;
      }

      if ( output_opts & OUTPUT_DEG ) {
        obj->ra *= 180 / M_PI;
        obj->dec *= 180 / M_PI;
      }

      if ( dump_object(output, output_opts, obj, i) != 0 ) {
        fprintf(stderr,"dump_object() fails\n");
        return 1;
      }
    }

  }

  if ( output != stdout ) {
    fclose(output);
  }

  return 0;
}

