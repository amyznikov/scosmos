/*
 * ssa-plate-stats.c
 *
 *  Created on: Feb 16, 2013
 *      Author: amyznikov
 */


#include "ssa-detection.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>

#define PI M_PI


static void show_usage( FILE * output )
{
  fprintf(output,"Gather some SuperCOSMOS plate statistics\n");
  fprintf(output,"USAGE:\n");
  fprintf(output,"   ssa-plate-stats [OPTIONS] [FILE]\n");
  fprintf(output,"OPTIONS:\n");
  fprintf(output,"   -j  treat input file as compressed by bzip2\n");
  fprintf(output,"   -z  treat input file as compressed by gzip\n");
  fprintf(output,"   -h  print headr line\n");
  fprintf(output,"If no input file is given then read plate file from stdin (to allow piped processing)\n");
  fprintf(output,"Examples:\n");
  fprintf(output," ssa-plate-stats 1-65537.dat.bz2\n");
}


static inline void minmax8( float8 v, float8 * vmin, float8 * vmax )
{
    if ( v < *vmin ) {
        *vmin = v;
    }
    else if ( v > *vmax ) {
        *vmax = v;
    }
}

int main(int argc, char *argv[])
{
  const char * inputfilename = NULL;
  FILE * input = stdin;

  ssa_detection2 obj;
  int i;

  enum {
    compression_none,
    compression_bzip,
    compression_gzip
  } compression = compression_none;

  int print_header_line = 0;

  double ramin=0, ramax=0, decmin=0, decmax=0;
  double xmin=0, xmax=0, ymin=0, ymax=0;
  double ra0, dec0, x0, y0, wx0 = 0, wy0 = 0;

  uint32_t numobj = 0;


  /* parse command line */
  for ( i = 1; i < argc; ++i )
  {
    if ( strcmp(argv[i],"--help") == 0 ) {
      show_usage(stdout);
      return 0;
    }

    if ( strcmp(argv[i],"-j") == 0 ) {
      compression = compression_bzip;
    }
    else if ( strcmp(argv[i],"-z") == 0 ) {
      compression = compression_gzip;
    }
    else if ( strcmp(argv[i],"-h") == 0 ) {
      print_header_line = 1;
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

  if ( compression && !inputfilename ) {
    fprintf(stderr,"error: decompression from stdin is not supported.\n"
        "Use zcat, bzcat, etc to pipe decompressed data\n");
    return 1;
  }


  /* open input file if requested */

  if ( inputfilename )
  {
    char cmd[PATH_MAX] = {0};

    if ( !compression )
    {
      const char * suffix;

      if ( (suffix = strstr(inputfilename, ".bz2")) && *(suffix + 4) == 0 ) {
        compression = compression_bzip;
      }
      else if ( (suffix = strstr(inputfilename, ".bz")) && *(suffix + 3) == 0 ) {
        compression = compression_bzip;
      }
      else if ( (suffix = strstr(inputfilename, ".gz")) && *(suffix + 3) == 0 ) {
        compression = compression_gzip;
      }
    }

    switch ( compression )
    {
    case compression_bzip:
      snprintf(cmd, sizeof(cmd) - 1, "bzip2 -dc '%s'", inputfilename);
      if ( !(input = popen(cmd, "r"))) {
        fprintf(stderr, "popen('%s') fails: %s\n", cmd, strerror(errno));
        return errno;
      }
      break;

    case compression_gzip:
      // snprintf(cmd, sizeof(cmd) - 1, "bash -o pipefail -c \"gzip -dc '%s'\"", inputfilename);
      snprintf(cmd, sizeof(cmd) - 1, "gzip -dc '%s'", inputfilename);
      if ( !(input = popen(cmd, "r"))) {
        fprintf(stderr, "popen('%s') fails: %s\n", cmd, strerror(errno));
        return errno;
      }
      break;

    case compression_none:
      if ( !(input = fopen(inputfilename, "r"))) {
        fprintf(stderr, "fopen('%s') fails: %s\n", inputfilename, strerror(errno));
        return errno;
      }
      break;

    default:
      fprintf(stderr, "BUG IN CODE: invalid compression tag=%d\n", compression);
      return 100;
    }
  }


  while ( fread(&obj, sizeof(obj), 1, input) == 1 )
  {
    if ( !numobj++ )
    {
      ramin = ramax = obj.ra;
      decmin = decmax = obj.dec;
      wx0 = xmin = xmax = obj.xCen;
      wy0 = ymin = ymax = obj.yCen;
    }
    else
    {
      /* special processing for ra wrapping */
      if ( fabs(obj.ra - ramax) > PI )
      {
        if ( ramax > PI ) {
          ramax -= 2 * PI;
        }
        else {
          obj.ra -= 2 * PI;
        }
      }

      minmax8(obj.ra, &ramin, &ramax);
      minmax8(obj.dec, &decmin, &decmax);
      minmax8(obj.xCen, &xmin, &xmax);
      minmax8(obj.yCen, &ymin, &ymax);

      wx0 += obj.xCen;
      wy0 += obj.yCen;
    }
  }


  if ( input != stdin ) {
    if( compression ) {
      pclose(input);
    }
    else {
      fclose (input);
    }
  }

  if ( print_header_line ) {
     printf("N\tRAMIN\tRAMAX\tRA0\tDECMIN\tDECMAX\tDEC0\tRASZ\tDECSZ\tXMIN\tXMAX\tX0\tWX0\tYMIN\tYMAX\tY0\tWY0\tXSZ\tYSZ\n");
  }

  if ( numobj )
  {
    ra0 = (ramin + ramax) / 2;
    dec0 = (decmin + decmax) / 2;
    x0 = (xmin + xmax) / 2;
    y0 = (ymin + ymax) / 2;
    wx0 /= numobj;
    wy0 /= numobj;

    printf(
        "%8d\t"
        "%16.9f\t%16.9f\t%16.9f\t"
        "%+16.9f\t%+16.9f\t%+16.9f\t"
        "%6.3f\t%6.3f\t"
        "%16.2f\t%16.2f\t%16.2f\t%16.2f\t"
        "%16.2f\t%16.2f\t%16.2f\t%16.2f\t"
        "%16.2f\t%16.2f\n",

        numobj,

        ramin,
        ramax,
        ra0,

        decmin,
        decmax,
        dec0,

        (ramax - ramin) * 180 / PI,
        (decmax - decmin) * 180 / PI,

        xmin,
        xmax,
        x0,
        wx0,

        ymin,
        ymax,
        y0,
        wy0,

        xmax - xmin,
        ymax - ymin
        );
  }

  return 0;
}

