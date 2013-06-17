/*
 * ssa-source-dump.c
 *
 *  Created on: Apr 18, 2013
 *      Author: amyznikov
 */


#include "ssa-source.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>

#define PI                      M_PI
#define INCLUDE_COLUMNS_HEADER  1
#define INCLUDE_RECORD_INDEX    2
#define CHECK_RAMIN    			4
#define CHECK_RAMAX    			8
#define CHECK_DECMIN    		16
#define CHECK_DECMAX    		32
#define CHECK_MAGMIN    		64
#define CHECK_MAGMAX    		128


static void show_usage( FILE * output )
{
  fprintf(output,"Dump SuperCOSMOS binary 'source' file to stdout as text\n");
  fprintf(output,"USAGE:\n");
  fprintf(output,"   ssa-source-dump [OPTIONS] [FILE]\n");
  fprintf(output,"OPTIONS:\n");
  fprintf(output,"   -h  include columns header\n");
  fprintf(output,"   -r  include (one-based) record index\n");
  fprintf(output,"   minmag=float  minimal (bright) output magnitude\n");
  fprintf(output,"   maxmag=float  maximal (faint) output magnitude\n");
  fprintf(output,"If no input file is given then read plate file from stdin (to allow piped processing)\n");
  fprintf(output,"Examples:\n");
  fprintf(output," ssa-source-dump -h ssaSource000ra030.bin\n");
}


int main(int argc, char *argv[])
{
  const char * inputfilename = NULL;
  int input = STDIN_FILENO;

  int output_options = 0;
  int i;

  ssa_source obj;

  double ramin, ramax, decmin, decmax;
//  double minmag, maxmag;



  for ( i = 1; i < argc; ++i )
  {
    if ( strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-help") == 0 ) {
      show_usage(stdout);
      return 0;
    }

    if ( *argv[i] == '-' )
    {
      const char * p = argv[i] + 1;
      for ( ; *p ; ++p )
      {
        switch ( *p )
        {
        case 'h' :
          output_options |= INCLUDE_COLUMNS_HEADER;
          break;
        case 'r' :
          output_options |= INCLUDE_RECORD_INDEX;
          break;
        default:
          fprintf(stderr,"Unknow option '%c' in %s\n", *p, argv[i] );
          return -1;
        }
      }
    }
    else if ( strncmp(argv[i],"ramin=",6) == 0)
    {
		if ( sscanf(argv[i]+6,"%lf", &ramin) != 1 ) {
			fprintf(stderr,"Invalid value of %s.\n", argv[i]);
			return -1;
		}
		output_options |= CHECK_RAMIN;
    }
    else if ( strncmp(argv[i],"ramax=",6) == 0)
    {
		if ( sscanf(argv[i]+6,"%lf", &ramax) != 1 ) {
			fprintf(stderr,"Invalid value of %s.\n", argv[i]);
			return -1;
		}
		output_options |= CHECK_RAMAX;
    }
    else if ( strncmp(argv[i],"decmin=",7) == 0)
    {
		if ( sscanf(argv[i]+7,"%lf", &decmin) != 1 ) {
			fprintf(stderr,"Invalid value of %s.\n", argv[i]);
			return -1;
		}
		output_options |= CHECK_DECMIN;
    }
    else if ( strncmp(argv[i],"decmax=",7) == 0)
    {
		if ( sscanf(argv[i]+7,"%lf", &decmax) != 1 ) {
			fprintf(stderr,"Invalid value of %s.\n", argv[i]);
			return -1;
		}
		output_options |= CHECK_DECMAX;
    }
    else if ( !inputfilename )
    {
      inputfilename = argv[i];
    }
    else {
      fprintf(stderr, "Invalid argument '%s'.\nNote that only single input file is accepted at once.", argv[i]);
      return -1;
    }
  }

  /* open input file if requested */
  if ( inputfilename && (input = open(inputfilename, O_RDONLY)) == -1 ) {
    fprintf(stderr, "Can't read '%s': %s\n", inputfilename, strerror(errno));
    return errno;
  }

  /* print header line */
  printf(
    "objID\t"
    "objIDB\t"
    "objIDR1\t"
    "objIDR2\t"
    "objIDI\t"
//    "htmId\t"
    "epoch\t"
    "ra\t"
    "dec\t"
    "sigRA\t"
    "sigDec\t"
//    "cx\t"
//    "cy\t"
//    "cz\t"
    "mura\t"
    "mudec\t"
    "emura\t"
    "emudec\t"
    "chi2\t"
    "Nplates\t"
    "classMagB\t"
    "classMagR1\t"
    "classMagR2\t"
    "classMagI\t"
    "gCorMagB\t"
    "gCorMagR1\t"
    "gCorMagR2\t"
    "gCorMagI\t"
    "sCorMagB\t"
    "sCorMagR1\t"
    "sCorMagR2\t"
    "sCorMagI\t"
    "meanClass\t"
    "classB\t"
    "classR1\t"
    "classR2\t"
    "classI\t"
    "ellipB\t"
    "ellipR1\t"
    "ellipR2\t"
    "ellipI\t"
    "qualB\t"
    "qualR1\t"
    "qualR2\t"
    "qualI\t"
    "blendB\t"
    "blendR1\t"
    "blendR2\t"
    "blendI\t"
    "prfStatB\t"
    "prfStatR1\t"
    "prfStatR2\t"
    "prfStatI\t"
    "l\t"
    "b\t"
    "d\t"
    "Ebmv\n"
  );


  while ( read(input, &obj, sizeof(obj)) == sizeof(obj) )
  {
    if ((output_options & CHECK_RAMIN) && obj.ra < ramin) {
      continue;
    }

    if ((output_options & CHECK_RAMAX) && obj.ra > ramax) {
      continue;
    }

    if ((output_options & CHECK_DECMIN) && obj.dec < decmin) {
      continue;
    }

    if ((output_options & CHECK_DECMAX) && obj.dec > decmax) {
      continue;
    }

    if ( fabs(obj.muAcosD) > 10000 || fabs(obj.muD) > 10000)
    {
      continue;
    }

    printf(
      "%16"PRId64"\t"   /* objID */
      "%16"PRId64"\t"   /* objIDB */
      "%16"PRId64"\t"   /* objIDR1 */
      "%16"PRId64"\t"   /* objIDR2 */
      "%16"PRId64"\t"   /* objIDI */
      //    "%16"PRId64"\t" /* htmId */
      "%8.3f\t"         /* epoch */
      "%15.9f\t"        /* ra */
      "%+15.9f\t"       /* dec */
      "%+8.3e\t"        /* sigRA */
      "%+8.3e\t"        /* sigDec */
      //    "float8\t"      /* cx */
      //    "float8\t"      /* cy */
      //    "float8\t"      /* cz */
      "%+9.3e\t"        /* muAcosD */
      "%+9.3e\t"        /* muD */
      "%+9.3e\t"        /* sigMuAcosD */
      "%+9.3e\t"        /* sigMuD */
      "%+9.5e\t"        /* chi2 */
      "%3d\t"           /* Nplates */
      "%+8.3f\t"        /* classMagB */
      "%+8.3f\t"        /* classMagR1 */
      "%+8.3f\t"        /* classMagR2 */
      "%+8.3f\t"        /* classMagI */
      "%+8.3f\t"        /* gCorMagB */
      "%+8.3f\t"        /* gCorMagR1 */
      "%+8.3f\t"        /* gCorMagR2 */
      "%+8.3f\t"        /* gCorMagI */
      "%+8.3f\t"        /* sCorMagB */
      "%+8.3f\t"        /* sCorMagR1 */
      "%+8.3f\t"        /* sCorMagR2 */
      "%+8.3f\t"        /* sCorMagI */
      "%2d\t"           /* meanClass */
      "%2d\t"           /* classB */
      "%2d\t"           /* classR1 */
      "%2d\t"           /* classR2 */
      "%2d\t"           /* classI */
      "%+12.9e\t"       /* ellipB */
      "%+12.9e\t"       /* ellipR1 */
      "%+12.9e\t"       /* ellipR2 */
      "%+12.9e\t"       /* ellipI */
      "%+9d\t"           /* qualB */
      "%+9d\t"           /* qualR1 */
      "%+9d\t"           /* qualR2 */
      "%+9d\t"           /* qualI */
      "%+9d\t"           /* blendB */
      "%+9d\t"           /* blendR1 */
      "%+9d\t"           /* blendR2 */
      "%+9d\t"           /* blendI */
      "%+12.9e\t"       /* prfStatB */
      "%+12.9e\t"       /* prfStatR1 */
      "%+12.9e\t"       /* prfStatR2 */
      "%+12.9e\t"       /* prfStatI */
      "%+15.9f\t"       /* l */
      "%+15.9f\t"       /* b */
      "%+15.9f\t"       /* d */
      "%+8.3f\n",       /* Ebmv */

      obj.objID,
      obj.objIDB,
      obj.objIDR1,
      obj.objIDR2,
      obj.objIDI,
      //      obj.htmId,
      obj.epoch,
      obj.ra * PI / 180,
      obj.dec * PI / 180,
      obj.sigRA * 3600 * cos(obj.dec * PI / 180),
      obj.sigDec * 3600,
      //      obj.cx,
      //      obj.cy,
      //      obj.cz,
      obj.muAcosD,
      obj.muD,
      obj.sigMuAcosD,
      obj.sigMuD,
      obj.chi2,
      obj.Nplates,
      obj.classMagB,
      obj.classMagR1,
      obj.classMagR2,
      obj.classMagI,
      obj.gCorMagB,
      obj.gCorMagR1,
      obj.gCorMagR2,
      obj.gCorMagI,
      obj.sCorMagB,
      obj.sCorMagR1,
      obj.sCorMagR2,
      obj.sCorMagI,
      obj.meanClass,
      obj.classB,
      obj.classR1,
      obj.classR2,
      obj.classI,
      obj.ellipB,
      obj.ellipR1,
      obj.ellipR2,
      obj.ellipI,
      obj.qualB,
      obj.qualR1,
      obj.qualR2,
      obj.qualI,
      obj.blendB,
      obj.blendR1,
      obj.blendR2,
      obj.blendI,
      obj.prfStatB,
      obj.prfStatR1,
      obj.prfStatR2,
      obj.prfStatI,
      obj.l,
      obj.b,
      obj.d,
      obj.Ebmv
    );

  }

  if ( input != STDIN_FILENO ) {
    close (input);
  }

  return 0;
}
