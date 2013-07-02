/*
 * ssa-pair-stars.c
 *
 *  Created on: June 18, 2013
 *      Author: amyznikov
 */


#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include "ccarray.h"

#define UNUSED(x)               ((void)(x))
#define MAX_HEADER_LENGTH       2048
#define MAX_INPUT_LINE_LENGTH   4096
#define PI                      M_PI
#define MAX_PAIRS               64

/** known compression types */
typedef
enum compression_t {
    compression_unknown = -1,
    compression_none,
    compression_bzip,
    compression_gzip
} compression_t;

/** RA and DC measure units */
enum {
  radians,
  degrees,
};

enum {
  dups_drop,
  dups_keep,
  dups_nearest,
};


/** object data */
typedef
struct obj_t {
  double ra, dec;       /*< in radians */
  char * line;
} obj_t;





/** uses fopen() if input file seems to be uncompresssed, and popen() if input file seems compressed */
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
    if ( !(input = fopen(fname, "r")) ) {
      fprintf(stderr, "fopen('%s') fails: %s\n", fname, strerror(errno));
    }
    break;

  default:
    fprintf(stderr, "BUG IN CODE: invalid compression tag=%d\n", *compression);
    break;
  }

  return input;
}

/** uses fclose() if input file was uncompressed, and pclose() for compressed case */
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

/* load lines from input file */
static int load_objects(FILE * input, int rc, int dc, int ru, int du, char header[MAX_HEADER_LENGTH],
    ccarray_t * objects)
{
  char line[MAX_INPUT_LINE_LENGTH];
  int ic;
  char * pc;
  size_t size;
  size_t capacity;

  obj_t * obj;


  header[MAX_HEADER_LENGTH-1] = 0;

  if ( !fgets(header, MAX_HEADER_LENGTH, input) ) {
    fprintf(stderr,"fgets(header) fails: %d (%s)\n", errno, strerror(errno));
    return -1;
  }

  if ( header[MAX_HEADER_LENGTH - 1] != 0 ) {
    fprintf(stderr,"too long header line in this file\n");
    return -1;
  }

  /* remove trailing new line */
  header[strlen(header) - 1] = 0;


  size = ccarray_size(objects);
  capacity = ccarray_capacity(objects);

  while ( size < capacity && !feof(input) )
  {
    obj = ccarray_peek(objects, size);

    line[MAX_INPUT_LINE_LENGTH-1] = 0;
    if ( !fgets(line, MAX_INPUT_LINE_LENGTH, input) ) {
      break;
    }

    if ( line[MAX_INPUT_LINE_LENGTH - 1] != 0 ) {
      fprintf(stderr,"too long input line in this file\n");
      return -1;
    }

    /* remove trailing new line */
    line[strlen(line) - 1] = 0;



    for ( pc = line, ic = 1; ic < rc; ++ic ) {
      if ( !(pc = strchr(pc + 1, '\t')) ) {
        break;
      }
    }
    if ( ic != rc || sscanf(pc, " %lf", &obj->ra) != 1 ) {
      continue;
    }


    for ( pc = line, ic = 1; ic < dc; ++ic ) {
      if ( !(pc = strchr(pc + 1, '\t')) ) {
        break;
      }
    }
    if ( ic != dc || sscanf(pc, " %lf", &obj->dec) != 1 ) {
      continue;
    }

    if ( ru == degrees ) {
      obj->ra *= PI / 180;
    }

    if ( du == degrees ) {
      obj->dec *= PI / 180;
    }

    obj->line = strdup(line);
    ccarray_set_size(objects, ++size );
  }

  return 0;
}

static int cmpradec( const void * p1, const void * p2 )
{
  const obj_t * obj1 = p1;
  const obj_t * obj2 = p2;

  if ( obj1->ra < obj2->ra ) {
    return -1;
  }

  if ( obj1->ra > obj2->ra ) {
    return +1;
  }

  if ( obj1->dec < obj2->dec ) {
    return -1;
  }

  if ( obj1->dec > obj2->dec ) {
    return +1;
  }

  return 0;
}

static int cmpra( const void * p1, const void * p2 )
{
  const obj_t * obj1 = p1;
  const double * ra = p2;

  if ( obj1->ra < *ra ) {
    return -1;
  }

  if ( obj1->ra > *ra ) {
    return +1;
  }

  return 0;
}


/** show usage info */
static void show_usage( FILE * output, int argc, char * argv[] )
{
  fprintf(output, "STAR LIST PAIRING UTILITY\n");
  fprintf(output, "USAGE:\n");
  fprintf(output, "  %s OPTIONS FILE1 FILE2\n", basename(argv[0]));
  fprintf(output, "OPTIONS:\n");
  fprintf(output, "  rc1=<integer>      one-based colum number of RA column in first file\n");
  fprintf(output, "  rc2=<integer>      one-based colum number of RA column in second file\n");
  fprintf(output, "  dc1=<integer>      one-based colum number of DC column in first file\n");
  fprintf(output, "  dc2=<integer>      one-based colum number of DC column in second file\n");
  fprintf(output, "  r=<float>          pair radius in arcsec (circular window)\n");
  fprintf(output, "  ru1={radian|deg}   RA unit\n");
  fprintf(output, "  du1={radian|deg}   DC unit\n");
  fprintf(output, "  ru2={radian|deg}   RA unit\n");
  fprintf(output, "  du2={radian|deg}   DC unit\n");
  fprintf(output, "  cap1=<size_t>      set initial capacity of first star list\n");
  fprintf(output, "  cap2=<size_t>      set initial capacity of second star list\n");
  fprintf(output, "  s1=suffix1         Suffix to add to all column names of first file\n");
  fprintf(output, "  s2=suffix2         Suffix to add to all column names of second file\n");
  fprintf(output, "  dups={keep,drop}   What to do with multiple detections?\n");
  fprintf(output, "  -o <out-file-name> Set output file name\n");
  fprintf(output, "  -d                 Write coordinate differences in additional columns\n");
  fprintf(output, "  -i                 Invert match\n");
  fprintf(output, "  -v                 Print some diagnostic messages to stderr (verbose mode)\n");
  fprintf(output, "  \n");

  UNUSED(argc);
}

static int parse_unit( const char * name )
{
  size_t len = name ? strlen(name) : 0;

  if ( len )
  {
    if ( strncasecmp("radians", name, len) == 0 ) {
      return radians;
    }

    if ( strncasecmp("degrees", name, len) == 0 ) {
      return degrees;
    }
  }

  return -1;
}


/** main() */
int main(int argc, char *argv[])
{
  const char * fname[2] =
    { NULL, NULL };

  const char * outname = NULL;

  FILE * fp[2] =
    { NULL, NULL };

  FILE * output = stdout;

  compression_t compression[2] =
    { compression_unknown, compression_unknown };

  int rc[2] =
    { -1, -1 };

  int dc[2] =
    { -1, -1 };

  int ru[2] =
    { radians, radians };

  int du[2] =
    { radians, radians };

  ccarray_t * list[2] =
    { NULL, NULL };

  size_t capacity[2] =
    { 15000000, 15000000 };

  char suffix[2][64] =
    { {0}, {0} };


  char head[2][MAX_HEADER_LENGTH];

  int append_diffs = 0;
  int dups_mode = dups_drop;
  int beverbose = 0;
  int invert_match = 0;

  double r = -1;

  int i;
  size_t size1, size2, pos1;

  /* parse command line */

  for ( i = 1; i < argc; ++i )
  {
    if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0 ) {
      show_usage(stdout, argc, argv);
      return 0;
    }

    if ( strncmp(argv[i], "rc1=", 4) == 0 )
    {
      if ( sscanf(argv[i] + 4, "%d", &rc[0]) != 1 || rc[0] < 1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "rc2=", 4) == 0 )
    {
      if ( sscanf(argv[i] + 4, "%d", &rc[1]) != 1 || rc[1] < 1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "dc1=", 4) == 0 )
    {
      if ( sscanf(argv[i] + 4, "%d", &dc[0]) != 1 || dc[0] < 1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "dc2=", 4) == 0 )
    {
      if ( sscanf(argv[i] + 4, "%d", &dc[1]) != 1 || dc[1] < 1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "ru1=", 4) == 0 )
    {
      if ( (ru[0] = parse_unit(argv[i] + 4)) == -1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "ru2=", 4) == 0 )
    {
      if ( (ru[1] = parse_unit(argv[i] + 4)) == -1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "du1=", 4) == 0 )
    {
      if ( (du[0] = parse_unit(argv[i] + 4)) == -1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "du2=", 4) == 0 )
    {
      if ( (du[1] = parse_unit(argv[i] + 4)) == -1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "r=", 2) == 0 )
    {
      if ( sscanf(argv[i] + 2, "%lf", &r) != 1 || r <= 0 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "cap1=", 5) == 0 )
    {
      if ( sscanf(argv[i] + 5, "%zu", &capacity[0]) != 1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "cap2=", 5) == 0 )
    {
      if ( sscanf(argv[i] + 5, "%zu", &capacity[1]) != 1 ) {
        fprintf(stderr,"Invalid value of %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i], "s1=", 3) == 0 )
    {
      strncpy(suffix[0], argv[i] + 3, sizeof(suffix[0]) - 1);
    }
    else if ( strncmp(argv[i], "s2=", 3) == 0 )
    {
      strncpy(suffix[1], argv[i] + 3, sizeof(suffix[1]) - 1);
    }
    else if ( strncmp(argv[i], "dups=", 5) == 0 )
    {
      if ( strcmp(argv[i] + 5, "drop") == 0 ) {
        dups_mode = dups_drop;
      }
      else if ( strcmp(argv[i] + 5, "keep") == 0 ) {
        dups_mode = dups_keep;
      }
      else if ( strcmp(argv[i] + 5, "nearest") == 0 ) {
        dups_mode = dups_nearest;
      }
      else {
        fprintf(stderr,"Invalid value of dups mode %s\n", argv[i]);
        return 1;
      }
    }
    else if ( *argv[i] == '-' )
    {
      const char * opt = argv[i] + 1;

      if ( strcmp(opt,"o") == 0 ) {
        if ( ++i >= argc ) {
          fprintf(stderr,"ERROR: missing output file name after -o switch\n");
          return 1;
        }
        outname = argv[i];
        continue;
      }

      for ( ; *opt ; ++opt )
      {
        switch ( *opt ) {
        case 'i': invert_match = 1; break;
        case 'v': beverbose = 1; break;
        case 'd': append_diffs = 1; break;
        default : fprintf(stderr, "Invalid key '%c' in argument '%s'\n", *opt, argv[i]); return 1;
        }
      }
    }
    else if ( !fname[0] ) {
      fname[0] = argv[i];
    }
    else if ( !fname[1] ) {
      fname[1] = argv[i];
    }
    else {
      fprintf(stderr, "Invalid argument '%s'. Note that only 2 input files are allowed\n", argv[i]);
      return 1;
    }
  }



  /* check command line inputs */

  if ( !fname[0] || !fname[1] ) {
    fprintf(stderr,"Two input file names expected\n");
    show_usage(stderr, argc, argv);
    return 1;
  }

  if ( rc[0] < 1 ) {
    fprintf(stderr,"rc1 argument is mandatory\n");
    show_usage(stderr, argc, argv);
    return -1;
  }

  if ( rc[1] < 1 ) {
    fprintf(stderr,"rc2 argument is mandatory\n");
    show_usage(stderr, argc, argv);
    return 1;
  }

  if ( dc[0] < 1 ) {
    fprintf(stderr,"dc1 argument is mandatory\n");
    show_usage(stderr, argc, argv);
    return 1;
  }

  if ( dc[1] < 1 ) {
    fprintf(stderr,"dc2 argument is mandatory\n");
    show_usage(stderr, argc, argv);
    return 1;
  }

  if ( (r *= (PI / (180 * 3600))) <= 0 ) {
    fprintf(stderr,"r argument is mandatory\n");
    show_usage(stderr, argc, argv);
    return 1;
  }




  /* check if input files are readable */
  for ( i = 0; i < 2; ++i )
  {
    if ( access(fname[i], R_OK) != 0 ) {
      fprintf(stderr, "Can't read %s: %s\n", fname[i], strerror(errno));
      return 1;
    }
  }

  /* allocate memory storage */
  for ( i = 0; i < 2; ++i )
  {
    if ( !(list[i] = ccarray_create(capacity[i], sizeof(obj_t))) ) {
      fprintf(stderr, "ccarray_create(capacity=%zu) fails: %s\n", capacity[i], strerror(errno));
      return 1;
    }
  }


  /* load input files */
  for ( i = 0; i < 2; ++i )
  {
    if ( beverbose ) {
      fprintf(stderr,"loading %s....\n", fname[i]);
    }

    if ( !(fp[i] = open_file(fname[i], &compression[i])) ) {
      fprintf(stderr, "Can't read '%s': %s\n", fname[i], strerror(errno));
      return 1;
    }

    if ( load_objects(fp[i], rc[i], dc[i], ru[i], du[i], head[i], list[i]) != 0 ) {
      fprintf(stderr, "Can't load %s\n", fname[i]);
      return 1;
    }
    close_file( fp[i], compression[i] );

    if ( beverbose ) {
      fprintf(stderr,"%s: %zu rows\n", fname[i], ccarray_size(list[i]));
    }

    if ( ccarray_size(list[i]) == capacity[i] && !feof(fp[i]) ) {
      fprintf(stderr, "load_objects() fails for %s: Too many objects. Try increase capacity (currently %zu)\n",
          fname[i], capacity[i]);
      return 1;
    }
  }


  /* create output file */
  if ( outname && ! (output = fopen(outname,"w")) ) {
    fprintf(stderr, "Can't write %s: %s\n", outname, strerror(errno));
    return 1;
  }


  /* sort lists */
  for ( i = 0; i < 2; ++i )
  {
    if ( beverbose ) {
      fprintf(stderr, "sort %s....\n", fname[i]);
    }
    ccarray_sort(list[i], 0, ccarray_size(list[i]), cmpradec);
  }



  /* search pairs */
  if ( beverbose ) {
    fprintf(stderr,"search pairs...\n");
  }

  /* print header line */
  if ( 1 )
  {
    static const char delims[] = "\t";
    char * p;

    if ( (p = strtok(head[0], delims)) ) {
      fprintf(output, "%s%s", p, suffix[0]);
      while ( (p = strtok(NULL, delims)) ) {
        fprintf(output, "\t%s%s", p, suffix[0]);
      }
    }

    if ( (p = strtok(head[1], delims)) ) {
      fprintf(output, "\t%s%s", p, suffix[1]);
      while ( (p = strtok(NULL, delims)) ) {
        fprintf(output, "\t%s%s", p, suffix[1]);
      }
    }

    if ( append_diffs ) {
      fprintf(output, "\tdra\tddec\tdr");
    }

    fprintf(output, "\n");
  }

  size1 = ccarray_size(list[0]);
  size2 = ccarray_size(list[1]);

  for ( pos1 = 0; pos1 < size1; ++pos1 )
  {
    double dra[MAX_PAIRS];
    double ddec[MAX_PAIRS];
    double dr[MAX_PAIRS];
    const obj_t * pairs[MAX_PAIRS];
    size_t numpairs = 0;

    const obj_t * obj1 = ccarray_peek(list[0], pos1);
    const double rcd = r / cos(obj1->dec);
    const double ramin = obj1->ra - rcd;
    const double ramax = obj1->ra + rcd;

    size_t pos2 = ccarray_lowerbound(list[1], 0, size2, cmpra, &ramin);
    for ( ; pos2 < size2; ++pos2 )
    {
      const obj_t *  obj2 = ccarray_peek(list[1], pos2);

      if ( obj2->ra > ramax ) {
        break;
      }

      if ( numpairs == MAX_PAIRS ) {
        fprintf(stderr, "Warning: too many pairs for '%s'\n", obj1->line);
        break;
      }

      dra[numpairs] = (obj2->ra - obj1->ra) * cos((obj1->dec + obj2->dec) / 2);
      ddec[numpairs] = (obj2->dec - obj1->dec);

      if ( (dr[numpairs] = hypot(dra[numpairs], ddec[numpairs])) <= r ) {
        pairs[numpairs++] = obj2;
      }
    }

    if ( numpairs < 1 ) {
      if ( invert_match ) {
        fprintf(output, "%s\n", obj1->line);
      }
    }
    else
    {
      switch ( dups_mode )
      {
      case dups_drop:
        if ( numpairs == 1 )
        {
          fprintf(output, "%s\t%s", obj1->line, pairs[0]->line);

          if ( !append_diffs ) {
            fprintf(output, "\n");
          }
          else {
            fprintf(output, "\t%+9.3f\t%+9.3f\t%+9.3f\n", dra[0] * 180 * 3600 / PI, ddec[0] * 180 * 3600 / PI,
                dr[0] * 180 * 3600 / PI);
          }
        }
        break;

      case dups_keep:
        for ( pos2 = 0; pos2 < numpairs; ++pos2 )
        {
          fprintf(output, "%s\t%s", obj1->line, pairs[pos2]->line);

          if ( !append_diffs ) {
            fprintf(output, "\n");
          }
          else {
            fprintf(output, "%+9.3f\t%+9.3f\t%+9.3f\n", dra[pos2] * 180 * 3600 / PI, ddec[pos2] * 180 * 3600 / PI,
                dr[pos2] * 180 * 3600 / PI);
          }
        }
        break;

      case dups_nearest:
        break;
      }
    }

  }


  return 0;
}
