/*
 * extract-ssa-plates.c
 *
 *  Created on: Feb 12, 2013
 *      Author: amyznikov
 */


#include "ssa-detection.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>

#define MAX_PLATES    2000

static struct {
  int32_t plateID;
  int fd;
} plates[MAX_PLATES];

static int numplates = 0;


static void ssa_detection_convert( const ssa_detection * src, ssa_detection2 * dest)
{
  dest->objID = src->objID;
  dest->parentID = src->parentID;
  dest->ra = src->ra * M_PI / 180.0;
  dest->dec = src->dec * M_PI / 180.0;
  dest->xmin = src->xmin;
  dest->xmax = src->xmax;
  dest->ymin = src->ymin;
  dest->ymax = src->ymax;
  dest->area = src->area;
  dest->ipeak = src->ipeak;
  dest->cosmag = src->cosmag;
  dest->isky = src->isky;
  dest->xCen = src->xCen;
  dest->yCen = src->yCen;
  dest->aU = src->aU;
  dest->bU = src->bU;
  dest->thetaU = src->thetaU;
  dest->aI = src->aI;
  dest->bI = src->bI;
  dest->thetaI = src->thetaI;
  dest->class = src->class;
  dest->pa = src->pa;
  dest->ap1 = src->ap1;
  dest->ap2 = src->ap2;
  dest->ap3 = src->ap3;
  dest->ap4 = src->ap4;
  dest->ap5 = src->ap5;
  dest->ap6 = src->ap6;
  dest->ap7 = src->ap7;
  dest->ap8 = src->ap8;
  dest->blend = src->blend;
  dest->quality = src->quality;
  dest->prfStat = src->prfStat;
  dest->prfMag = src->prfMag;
  dest->gMag = src->gMag;
  dest->sMag = src->sMag;
}


static int get_plate_index( int32_t plateid )
{
  int index = 0;
  while ( index < numplates && plates[index].plateID != plateid ) {
    ++index;
  }
  return index;
}


static void show_usage( FILE * fp, int argc, char *argv[] )
{
  fprintf(fp,"Usage:\n");
  fprintf(fp,"   %s survey=<surveyid> [plate=<plateid>] [INPUT-FILE]\n", basename(argv[0]));
  fprintf(fp,"If no plateid is specified then all plates of given survey will extracted into separate output files\n");
}

int main(int argc, char *argv[])
{
  const char * inputfilename = NULL;
  int input = STDIN_FILENO;

  int surveyid = -1;
  int32_t requested_plateid = -1;

  ssa_detection obj1;
  ssa_detection2 obj2;
  int i;

  for ( i = 1; i < argc; ++i )
  {
    if ( strcmp(argv[i],"--help") == 0 ) {
      show_usage(stdout, argc, argv);
      return 0;
    }

    if ( strncmp(argv[i],"survey=", 7) == 0) {
      if ( sscanf(argv[i]+7,"%d", &surveyid) != 1 || surveyid < 0 || surveyid > 9 ) {
        fprintf(stderr,"Invalid surveyid %s\n", argv[i]);
        return 1;
      }
    }
    else if ( strncmp(argv[i],"plate=", 6) == 0) {
      if ( sscanf(argv[i] + 6, "%d", &requested_plateid) != 1 || requested_plateid < 0 ) {
        fprintf(stderr,"Invalid plateid %s\n", argv[i]);
        return 1;
      }
    }
    else if ( inputfilename == NULL ) {
      inputfilename = argv[i];
    }
    else {
      fprintf(stderr, "Too many input file names\n");
      show_usage(stderr,argc, argv);
      return 1;
    }
  }

  if ( surveyid == -1 ) {
    fprintf(stderr,"no surveyid given\n");
    show_usage(stderr,argc, argv);
    return 1;
  }

  if ( inputfilename && (input = open(inputfilename, O_RDONLY)) == -1 ) {
    fprintf(stderr, "Can't read '%s': %s\n", inputfilename, strerror(errno));
    return 1;
  }

  while ( read(input, &obj1, sizeof(obj1)) == sizeof(obj1) )
  {
    if ( obj1.surveyID == surveyid )
    {
      int32_t plateid = obj1.plateID;
      int plate_index;

      if ( requested_plateid != -1 && plateid != requested_plateid ) {
        /* skip this object */
        continue;
      }

      plate_index = get_plate_index(plateid);

      if ( plate_index == numplates )
      {
        const int oflags = O_APPEND | O_WRONLY | O_CREAT;
        const mode_t omode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
        char fname[256];

        if ( numplates >= MAX_PLATES ) {
          fprintf(stderr, "FATAL ERROR: too many plates. Increate the MAX_PLATES parameter in source code\n");
          return 2;
        }

        sprintf(fname,"%d-%d.dat", surveyid, plateid);

        fprintf(stderr, "creating %s\n", fname);
        plates[numplates].fd = open(fname, oflags, omode);
        plates[numplates].plateID = plateid;

        if ( plates[numplates].fd == -1 ) {
          fprintf(stderr, "Can't create '%s': %s\n", fname, strerror(errno));
          return 3;
        }

        plate_index = numplates ++;
        fprintf(stderr,"numplates=%d\n", numplates);
      }

      ssa_detection_convert(&obj1, &obj2);

      if ( write(plates[plate_index].fd, &obj2, sizeof(obj2)) != sizeof(obj2) ) {
        fprintf(stderr, "Can't write '%d-%d.dat': %s\n", surveyid, plateid, strerror(errno));
        return 4;
      }
    }
  }

  for ( i = 0; i < numplates; ++i ) {
    if ( plates[i].fd != -1 ) {
      close(plates[i].fd);
    }
  }

  if ( input != STDIN_FILENO ) {
    close(input);
  }

  return 0;
}
