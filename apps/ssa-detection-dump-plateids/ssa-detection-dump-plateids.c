/*
 * get-survey-plates.c
 *
 *  Created on: Feb 13, 2013
 *      Author: amyznikov
 */


#include "ssa-detection.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>

#define MAX_SURVEYS     10
#define MAX_PLATES    1000

typedef
struct plate {
  int32_t   plateid;                /* unique plate id */
  uint32_t  objcount;               /* count of objects on this plate */
} plate;


typedef
struct survey {
  int       surveyid;               /* unique survey id */
  int       numplates;              /* count of plates in this survey */
  plate     plates[MAX_PLATES];     /* plates array */
} survey;



/**
 * Get the index of item in surveys[] array where surveys[index].surveyid = surveyid;
 * returns 'numsurveys' if 'surveyid' not found in surveys[] array
 */
static int get_survey_index( const survey surveys[], int numsurveys, int surveyid )
{
  int index = 0;
  while ( index < numsurveys && surveys[index].surveyid != surveyid ) {
    ++index;
  }
  return index;
}


/**
 * get the index of item in plates[] array where plateID = plateid
 * returns 'numplates' if 'plateid' not found in plates[] array
 */
static int get_plate_index( const plate plates[], int numplates, int32_t plateid )
{
  int index = 0;
  while ( index < numplates && plates[index].plateid != plateid ) {
    ++index;
  }
  return index;
}




static void show_usage( FILE * output )
{
  fprintf(output,"Scan SuperCOSMOS binary plate file and print distinct plateid's present in file\n");
  fprintf(output,"Usage:\n");
  fprintf(output,"   ssa-detection-dump-plateids [SuperCOSMOS-Binary-File]\n");
  fprintf(output,"Example:\n");
  fprintf(output," ssa-detection-dump-plateids ssadetection000ra030.bin\n");
  fprintf(output,"If no INPUT-FILE is given then read plate file from stdin (to allow piped processing)\n");
}

int main(int argc, char *argv[])
{
  const char * inputfilename = NULL;
  int input = STDIN_FILENO;

  static survey surveys[MAX_SURVEYS];
  static int numsurveys = 0;          /* number of unique survey id's in input file */

  ssa_detection obj;
  uint64_t recnum;

  int i, j;

  /* parse command line */
  for ( i = 1; i < argc; ++i )
  {
    if ( strcmp(argv[i],"--help") == 0 ) {
      show_usage(stdout);
      return 0;
    }

    if ( inputfilename == NULL ) {
      inputfilename = argv[i];
    }
    else {
      fprintf(stderr, "Too many input file names\n");
      show_usage(stderr);
      return 1;
    }
  }


  /* open input file if requested */
  if ( inputfilename && (input = open(inputfilename, O_RDONLY)) == -1 ) {
    fprintf(stderr, "Can't read '%s': %s\n", inputfilename, strerror(errno));
    return 1;
  }


  /* scan whole file and count objects  */

  recnum = 0;
  while ( read(input, &obj, sizeof(obj)) == sizeof(obj) )
  {
    int surveyid = obj.surveyID;
    int32_t plateid = obj.plateID;

    int survey_index;
    int plate_index;

    if ( surveyid < 0 || surveyid > 9 || plateid < 0 )
    {
      fprintf(stderr, "FATAL ERROR: invalid surveyid=%d plateid=%d at record=%llu offset=%llu.\n",
          surveyid,
          plateid,
          (unsigned long long)recnum,
          (unsigned long long)(recnum * sizeof(obj)));

      fprintf(stderr, "partial dump of invalid record:\n");
      fprintf(stderr, "objID\tsurveyID\tplateID\tparentID\tsourceID\trecNum\tra\tdec\thtmId\n");
      fprintf(stderr, "%lld\t%d\t%d\t%lld\t%lld\t%d\t%16.9f\t%+16.9f\t%lld\n",
          obj.objID,obj.surveyID,obj.plateID,obj.parentID,obj.sourceID,obj.recNum,obj.ra,obj.dec,obj.htmId);
      return 2;
    }


    /* search survey slot */
    survey_index = get_survey_index(surveys, numsurveys, surveyid);
    if ( survey_index == numsurveys )
    {
      if ( numsurveys >= MAX_SURVEYS ) {
        fprintf(stderr, "FATAL ERROR: too many distinct surveyid's in this file.\n"
            "Increate the MAX_SURVEYS parameter in this source code"
            "if you really sure that input file has correct SuperCOSMOS binary format\n");
        return 2;
      }

      survey_index = numsurveys ++;
      surveys[survey_index].surveyid = surveyid;
      surveys[survey_index].numplates = 0;
    }


    /* search plate slot */
    plate_index = get_plate_index(surveys[survey_index].plates, surveys[survey_index].numplates, plateid);
    if ( plate_index == surveys[survey_index].numplates )
    {
      if ( surveys[survey_index].numplates >= MAX_PLATES )
      {
        fprintf(stderr, "FATAL ERROR: too many distinct plateid's in this file.\n"
            "Increate the MAX_PLATES parameter in this source code"
            "if you really sure that input file has correct SuperCOSMOS binary format\n");

        fprintf(stderr, "recnum=%llu offset=%llu",
            (unsigned long long)recnum,
            (unsigned long long)(recnum * sizeof(obj)));

        return 2;
      }

      plate_index = surveys[survey_index].numplates ++;
      surveys[survey_index].plates[plate_index].plateid = plateid;
      surveys[survey_index].plates[plate_index].objcount = 0;
    }

    ++ surveys[survey_index].plates[plate_index].objcount;
    ++ recnum;
  }

  if ( input != STDIN_FILENO ) {
    close(input);
  }


  printf("surveyid\tplateid\tnumObjects\n");
  for ( i = 0; i < numsurveys; ++i ) {
    for ( j = 0; j < surveys[i].numplates; ++j ) {
      printf("%6d\t%6d\t%6d\n", surveys[i].surveyid, surveys[i].plates[j].plateid, surveys[i].plates[j].objcount);
    }
  }


  return 0;
}
