/*
 * ssa-source.h
 *
 *  Created on: Apr 18, 2013
 *      Author: amyznikov
 */

#ifndef __ssa_source_h__
#define __ssa_source_h__

#include <stddef.h>
#include <stdint.h>


typedef float float4;
typedef double float8;


#pragma pack(push,1)
typedef
struct ssa_source {
  int64_t objID;      /*<         Unique identifier of merged source*/
  int64_t objIDB;     /*<         objID for B band detection merged into this object  0 */
  int64_t objIDR1;    /*<         objID for R1 band detection merged into this object 0 */
  int64_t objIDR2;    /*<         objID for R2 band detection merged into this object 0 */
  int64_t objIDI;     /*<         objID for I band detection merged into this object  0 */
  int64_t htmId;      /*<         Hierarchical Triangular Mesh (20-deep) of centroid    */
  float4  epoch;      /*< year    Epoch of position (variance weighted mean epoch of available measures) */
  float8  ra;         /*< degrees Mean RA, computed from detections merged in this catalogue */
  float8  dec;        /*< degrees Mean Dec, computed from detections merged in this catalogue */
  float8  sigRA;      /*< degrees Uncertainty in RA (formal random error not inc. systematic errors) */
  float8  sigDec;     /*< degrees Uncertainty in Dec (formal random error not inc. systematic errors) */
  float8  cx;         /*<         Cartesian x of unit (ra,dec) vector on celestial sphere */
  float8  cy;         /*<         Cartesian y of unit (ra,dec) vector on celestial sphere */
  float8  cz;         /*<         Cartesian z of unit (ra,dec) vector on celestial sphere */
  float4  muAcosD;    /*< mas/yr  Proper motion in RA direction -0.99999995e9 */
  float4  muD;        /*< mas/yr  Proper motion in Dec direction  -0.99999995e9 */
  float4  sigMuAcosD; /*< mas/yr  Error on proper motion in RA direction  -0.99999995e9 */
  float4  sigMuD;     /*< mas/yr  Error on proper motion in Dec direction -0.99999995e9 */
  float4  chi2;       /*<         Chi-squared value of proper motion solution -0.99999995e9 */
  int8_t  Nplates;    /*<         No. of plates used for this proper motion measurement */
  float4  classMagB;  /*< mag     B band magnitude selected by B image class  -99.999995 */
  float4  classMagR1; /*< mag     R1 band magnitude selected by R1 image class  -99.999995 */
  float4  classMagR2; /*< mag     R2 band magnitude selected by R2 image class  -99.999995 */
  float4  classMagI;  /*< mag     I band magnitude selected by I image class  -99.999995 */
  float4  gCorMagB;   /*< mag     B band magnitude assuming object is galaxy  -99.999995 */
  float4  gCorMagR1;  /*< mag     R1 band magnitude assuming object is galaxy -99.999995 */
  float4  gCorMagR2;  /*< mag     R2 band magnitude assuming object is galaxy -99.999995 */
  float4  gCorMagI;   /*< mag     I band magnitude assuming object is galaxy  -99.999995 */
  float4  sCorMagB;   /*< mag     B band magnitude assuming object is star  -99.999995 */
  float4  sCorMagR1;  /*< mag     R1 band magnitude assuming object is star -99.999995 */
  float4  sCorMagR2;  /*< mag     R2 band magnitude assuming object is star -99.999995 */
  float4  sCorMagI;   /*< mag     I band magnitude assuming object is star  -99.999995 */
  int8_t  meanClass;  /*<         Estimate of image class based on unit-weighted mean of individual classes */
  int8_t  classB;     /*<         Image classification from B band detection  0 */
  int8_t  classR1;    /*<         Image classification from R1 band detection 0 */
  int8_t  classR2;    /*<         Image classification from R2 band detection 0 */
  int8_t  classI;     /*<         Image classification from I band detection  0 */
  float4  ellipB;     /*<         Ellipticity of B band detection -0.99999995e9 */
  float4  ellipR1;    /*<         Ellipticity of R1 band detection  -0.99999995e9 */
  float4  ellipR2;    /*<         Ellipticity of R2 band detection  -0.99999995e9 */
  float4  ellipI;     /*<         Ellipticity of I band detection -0.99999995e9 */
  int32_t qualB;      /*<         Bitwise quality flag from B band detection  -1 */
  int32_t qualR1;     /*<         Bitwise quality flag from R1 band detection -1 */
  int32_t qualR2;     /*<         Bitwise quality flag from R2 band detection -1 */
  int32_t qualI;      /*<         Bitwise quality flag from I band detection  -1 */
  int32_t blendB;     /*<         Blend flag from B band detection  -99999999 */
  int32_t blendR1;    /*<         Blend flag from R1 band detection -99999999 */
  int32_t blendR2;    /*<         Blend flag from R2 band detection -99999999 */
  int32_t blendI;     /*<         Blend flag from I band detection  -99999999 */
  float4  prfStatB;   /*< N(0,1)  Profile statistic from B band detection -0.99999995e9 CLASS_STAR/GALAXY */
  float4  prfStatR1;  /*< N(0,1)  Profile statistic from R1 band detection  -0.99999995e9 CLASS_STAR/GALAXY */
  float4  prfStatR2;  /*< N(0,1)  Profile statistic from R2 band detection  -0.99999995e9 CLASS_STAR/GALAXY */
  float4  prfStatI;   /*< N(0,1)  Profile statistic from I band detection -0.99999995e9 CLASS_STAR/GALAXY */
  float4  l;          /*< degrees The Galactic longitude of the source */
  float4  b;          /*< degrees The Galactic latitude of the source */
  float4  d;          /*< degrees The great-circle distance of the source from the Galactic centre */
  float4  Ebmv;       /*< mag The estimated foreground reddening at this position from Schlegel et al. (1998) */
} ssa_source;
#pragma pack(pop)

#endif /* __ssa_source_h__ */
