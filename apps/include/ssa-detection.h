/*
 * ssa-detection.h
 *
 *  Created on: Feb 12, 2013
 *      Author: amyznikov
 *
 */

#ifndef __ssa_detection_h__
#define __ssa_detection_h__

#include <stddef.h>
#include <stdint.h>


typedef float float4;
typedef double float8;

#pragma pack(push,1)
/**
 * See http://surveys.roe.ac.uk/ssa/www/SSA_TABLE_DetectionSchema.html#Detection
 * The whole detection table is here in RA sections http://www-wfau.roe.ac.uk/www-data/ssa-detection/
 */
typedef
struct ssa_detection_s {
  // Type  Name         Length  Unit  Description Default Value Unified Content Descriptor
  int64_t objID;      // 8   Unique object identification number   ID_MAIN
  int8_t  surveyID;   // 1   The survey that collected the data    ID_SURVEY
  int32_t plateID ;   // 4   Plate ID of the plate that this detection came from   ID_PLATE
  int64_t parentID ;  // 8   objID of parent object, = objID for parent/undeblended (ie. not child) object   ID_PARENT
  int64_t sourceID ;  // 8   objID for corresponding merged record in Source   ID_ASSOCIATED
  int32_t recNum ;    // 4   The record number in the original IAM file    RECORD
  float8  ra;         // 8 degrees Celestial Right Ascension   POS_EQ_RA_MAIN
  float8  dec;        // 8 degrees Celestial Declination   POS_EQ_DEC_MAIN
  int64_t htmId ;     // 8   Hierarchical Triangular Mesh (20-deep) of centroid    POS_GENERAL
  float8  cx ;        // 8   Cartesian x of unit (ra,dec) vector on celestial sphere   POS_EQ_X
  float8  cy ;        // 8   Cartesian y of unit (ra,dec) vector on celestial sphere   POS_EQ_Y
  float8  cz ;        // 8   Cartesian z of unit (ra,dec) vector on celestial sphere   POS_EQ_Z
  float8  xmin ;      // 8 microns Leftmost extent of image on plate   POS_PLATE_X
  float8  xmax ;      // 8 microns Rightmost extent of image on plate    POS_PLATE_X
  float8  ymin ;      // 8 microns Bottommost extent of image on plate   POS_PLATE_Y
  float8  ymax ;      // 8 microns Topmost extent of image on plate    POS_PLATE_Y
  int32_t area ;      // 4 pixels  Area within detection isophote    EXTENSION_AREA
  float4  ipeak ;     // 4 instrumental units  Intensity of brightest pixel above sky    PHOT_INTENSITY_MISC
  float4  cosmag ;    // 4 instrumental units  Isophotal magnitude   PHOT_PHG_MAG
  float4  isky ;      // 4   Estimate of sky intensity at (xCen, yCen)   PHOT_SB_SKY
  float8  xCen ;      // 8 microns Intensity-weighted X-centroid of image    POS_PLATE_X
  float8  yCen ;      // 8 microns Intensity-weighted Y-centroid of image    POS_PLATE_Y
  float4  aU ;        // 4 microns Semi-major axis of unweighted fitted ellipse    EXTENSION_RAD
  float4  bU ;        // 4 microns Semi-minor axis of unweighted fitted ellipse    EXTENSION_SMIN
  int16_t thetaU ;    // 2 degrees Position angle (wrt x-axis) of unweighted fitted ellipse    POS_POS-ANG
  float4  aI ;        // 4 microns Semi-major axis of intensity-weighted fitted ellip.   EXTENSION_RAD
  float4  bI ;        // 4 microns Semi-minor axis of intensity-weighted fitted ellip.   EXTENSION_SMIN
  int16_t thetaI ;    // 2 degrees Position angle (wrt x-axis) of intensity-weighted fitted ellipse    POS_POS-ANG
  uint8_t class ;     // 1   Classification flag   CLASS_OBJECT
  int16_t pa ;        // 2 degrees Celestial position angle (E of N)   POS_POS-ANG
  int32_t ap1 ;       // 4 pixels  Area above 1st areal profile threshold    EXTENSION_AREA
  int32_t ap2 ;       // 4 pixels  Area above 2nd areal profile threshold    EXTENSION_AREA
  int32_t ap3 ;       // 4 pixels  Area above 3rd areal profile threshold    EXTENSION_AREA
  int32_t ap4 ;       // 4 pixels  Area above 4th areal profile threshold    EXTENSION_AREA
  int32_t ap5 ;       // 4 pixels  Area above 5th areal profile threshold    EXTENSION_AREA
  int32_t ap6 ;       // 4 pixels  Area above 6th areal profile threshold    EXTENSION_AREA
  int32_t ap7 ;       // 4 pixels  Area above 7th areal profile threshold    EXTENSION_AREA
  int32_t ap8 ;       // 4 pixels  Area above 8th areal profile threshold    EXTENSION_AREA
  int32_t blend ;     // 4   Deblend flag: -n=parent of n children; +n = nth child of a parent   CODE_MULT_INDEX
  int32_t quality ;   // 4   Bitwise quality flag    CODE_QUALITY
  float4  prfStat ;   // 4 N(0,1)  Profile statistic   CLASS_STAR/GALAXY
  float4  prfMag ;    // 4 instrumental units  Profile magnitude   PHOT_PHG_MAG
  float4  gMag ;      // 4 mag Magnitude calculated on assumption object is galaxy   MODEL_MAG_VALUE
  float4  sMag ;      // 4 mag Magnitude calculated on assumption object is star   MODEL_MAG_VALUE
  int16_t SSAfield ;  // 2   SSS field number (ESO/SRC system) that this detection falls in.   ID_FIELD
  int16_t seam ;      // 2   Seam index: = zero or SSAfield for seamless selection   ID_FIELD
} ssa_detection;
#pragma pack(pop)


#pragma pack(push,1)
/**
 * Selected subset of columns from ssa_detection_s structure,
 * contains data only for particular plate
 * */
typedef
struct ssa_detection2_s {
  // Type  Name         Length  Unit  Description Default Value Unified Content Descriptor
  int64_t objID;      // 8   Unique object identification number   ID_MAIN
  int64_t parentID ;  // 8   objID of parent object, = objID for parent/undeblended (ie. not child) object   ID_PARENT
  float8  ra;         // 8 degrees Celestial Right Ascension   POS_EQ_RA_MAIN
  float8  dec;        // 8 degrees Celestial Declination   POS_EQ_DEC_MAIN
  float8  xmin ;      // 8 microns Leftmost extent of image on plate   POS_PLATE_X
  float8  xmax ;      // 8 microns Rightmost extent of image on plate    POS_PLATE_X
  float8  ymin ;      // 8 microns Bottommost extent of image on plate   POS_PLATE_Y
  float8  ymax ;      // 8 microns Topmost extent of image on plate    POS_PLATE_Y
  int32_t area ;      // 4 pixels  Area within detection isophote    EXTENSION_AREA
  float4  ipeak ;     // 4 instrumental units  Intensity of brightest pixel above sky    PHOT_INTENSITY_MISC
  float4  cosmag ;    // 4 instrumental units  Isophotal magnitude   PHOT_PHG_MAG
  float4  isky ;      // 4   Estimate of sky intensity at (xCen, yCen)   PHOT_SB_SKY
  float8  xCen ;      // 8 microns Intensity-weighted X-centroid of image    POS_PLATE_X
  float8  yCen ;      // 8 microns Intensity-weighted Y-centroid of image    POS_PLATE_Y
  float4  aU ;        // 4 microns Semi-major axis of unweighted fitted ellipse    EXTENSION_RAD
  float4  bU ;        // 4 microns Semi-minor axis of unweighted fitted ellipse    EXTENSION_SMIN
  int16_t thetaU ;    // 2 degrees Position angle (wrt x-axis) of unweighted fitted ellipse    POS_POS-ANG
  float4  aI ;        // 4 microns Semi-major axis of intensity-weighted fitted ellip.   EXTENSION_RAD
  float4  bI ;        // 4 microns Semi-minor axis of intensity-weighted fitted ellip.   EXTENSION_SMIN
  int16_t thetaI ;    // 2 degrees Position angle (wrt x-axis) of intensity-weighted fitted ellipse    POS_POS-ANG
  uint8_t class ;     // 1   Classification flag   CLASS_OBJECT
  int16_t pa ;        // 2 degrees Celestial position angle (E of N)   POS_POS-ANG
  int32_t ap1 ;       // 4 pixels  Area above 1st areal profile threshold    EXTENSION_AREA
  int32_t ap2 ;       // 4 pixels  Area above 2nd areal profile threshold    EXTENSION_AREA
  int32_t ap3 ;       // 4 pixels  Area above 3rd areal profile threshold    EXTENSION_AREA
  int32_t ap4 ;       // 4 pixels  Area above 4th areal profile threshold    EXTENSION_AREA
  int32_t ap5 ;       // 4 pixels  Area above 5th areal profile threshold    EXTENSION_AREA
  int32_t ap6 ;       // 4 pixels  Area above 6th areal profile threshold    EXTENSION_AREA
  int32_t ap7 ;       // 4 pixels  Area above 7th areal profile threshold    EXTENSION_AREA
  int32_t ap8 ;       // 4 pixels  Area above 8th areal profile threshold    EXTENSION_AREA
  int32_t blend ;     // 4   Deblend flag: -n=parent of n children; +n = nth child of a parent   CODE_MULT_INDEX
  int32_t quality ;   // 4   Bitwise quality flag    CODE_QUALITY
  float4  prfStat ;   // 4 N(0,1)  Profile statistic   CLASS_STAR/GALAXY
  float4  prfMag ;    // 4 instrumental units  Profile magnitude   PHOT_PHG_MAG
  float4  gMag ;      // 4 mag Magnitude calculated on assumption object is galaxy   MODEL_MAG_VALUE
  float4  sMag ;      // 4 mag Magnitude calculated on assumption object is star   MODEL_MAG_VALUE
} ssa_detection2;
#pragma pack(pop)


#endif /* __ssa_detection_h__ */
