/*
 * ssa-detection-dump.c
 *
 *  Created on: Feb 14, 2013
 *      Author: amyznikov
 */


#define _LARGEFILE64_SOURCE     /* See man lseek64 */

#include "ssa-detection.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>



static void show_usage( FILE * output )
{
  fprintf(output,"Dump SuperCOSMOS binary file to stdout as text\n");
  fprintf(output,"USAGE:\n");
  fprintf(output,"   ssa-detection-dump [OPTIONS] [SuperCOSMOS-Binary-File.bin]\n");
  fprintf(output,"OPTIONS:\n");
  fprintf(output,"   startrec=int64   seek specified record position before starting read file\n");
  fprintf(output,"   startbyte=int64  seek specified byte position before starting read file\n");
  fprintf(output,"                  byte position will truncated to record boundary if need\n");
  fprintf(output,"If no input file is given then read binary data from stdin (to allow piped processing)\n");
  fprintf(output,"Examples:\n");
  fprintf(output," ssa-detection-dump startrec=12345 ssadetection000ra030.bin\n");
}


int main(int argc, char *argv[])
{
  const char * inputfilename = NULL;
  int input = STDIN_FILENO;

  ssa_detection obj;
  int64_t startrec = -1;
  int64_t startbyte = -1;
  int i;


  /* parse command line */
  for ( i = 1; i < argc; ++i )
  {
    if ( strcmp(argv[i],"--help") == 0 ) {
      show_usage(stdout);
      return 0;
    }

    if ( strncmp(argv[i],"startrec=",9) == 0 )
    {
      if ( sscanf(argv[i]+9, "%"PRId64"", &startrec) != 1 || startrec < 0 ) {
        fprintf(stderr,"Invalid value %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i],"startbyte=",10) == 0 )
    {
      if ( sscanf(argv[i]+10,"%"PRId64"", &startbyte) != 1 || startbyte < 0 ) {
        fprintf(stderr,"Invalid value %s\n", argv[i]);
        return 1;
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

  if ( startrec >=0 && startbyte >= 0 ) {
    fprintf(stderr, "Only one of startrec or startbyte may ber specified\n");
    show_usage(stderr);
    return 1;
  }


  /* adjust starting file offset if requested */
  if ( startrec > 0 ) {
    startbyte = startrec * sizeof(obj);
  }
  else if ( startbyte > 0 ) {
    startrec = startbyte / sizeof(obj);
    startbyte = startrec * sizeof(obj);
  }
  else {
    startbyte = 0;
    startrec = 0;
  }

  /* open input file if requested */
  if ( inputfilename && (input = open(inputfilename, O_RDONLY)) == -1 ) {
    fprintf(stderr, "Can't read '%s': %s\n", inputfilename, strerror(errno));
    return errno;
  }

  if ( startbyte > 0 )
  {
    fprintf(stderr, "NOTE: reading from byte offset=%"PRId64" (record index=%"PRId64")\n", startbyte, startrec );

    if ( lseek64(input, startbyte, SEEK_SET) != startbyte ) {
      fprintf(stderr, "ERROR: can't seek specified file position. errno=%d (%s)\n", errno, strerror(errno));
      close (input);
      return errno;
    }
  }

  /* print header line */
  printf("recnum\t"
        "objID\t"
        "surveyID\t"
        "plateID\t"
        "parentID\t"
        "sourceID\t"
        "recNum\t"
        "ra\t"
        "dec\t"
        "htmId\t"
        "cx\t"
        "cy\t"
        "cz\t"
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
        "sMag\t"
        "SSAfield\t"
        "seam\n"
        );

  while ( read(input, &obj, sizeof(obj)) == sizeof(obj) )
  {
    printf(
      "%16"PRId64"\t"
      "%16"PRId64"\t"
      "%3d\t"
      "%9d\t"
      "%16"PRId64"\t"
      "%16"PRId64"\t"
      "%9d\t"
      "%16.9f\t"
      "%+16.9f\t"
      "%16"PRId64"\t"
      "%16.9f\t"
      "%16.9f\t"
      "%16.9f\t"
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
      "%9.3f\t"
      "%6d\t"
      "%6d\n",
      startrec,
      obj.objID,
      obj.surveyID,
      obj.plateID,
      obj.parentID,
      obj.sourceID,
      obj.recNum,
      obj.ra,
      obj.dec,
      obj.htmId,
      obj.cx,
      obj.cy,
      obj.cz,
      obj.xmin,
      obj.xmax,
      obj.ymin,
      obj.ymax,
      obj.area,
      obj.ipeak,
      obj.cosmag,
      obj.isky,
      obj.xCen,
      obj.yCen,
      obj.aU,
      obj.bU,
      obj.thetaU,
      obj.aI,
      obj.bI,
      obj.thetaI,
      obj.class,
      obj.pa,
      obj.ap1,
      obj.ap2,
      obj.ap3,
      obj.ap4,
      obj.ap5,
      obj.ap6,
      obj.ap7,
      obj.ap8,
      obj.blend,
      obj.quality,
      obj.prfStat,
      obj.prfMag,
      obj.gMag,
      obj.sMag,
      obj.SSAfield,
      obj.seam
    );

    ++startrec;
  }

  if ( input != STDIN_FILENO ) {
    close (input);
  }

  return 0;
}
