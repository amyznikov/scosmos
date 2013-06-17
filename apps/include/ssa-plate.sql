drop table if exists ssa_plate;

create table ssa_plate (
  objID         int8,       --  8   Unique object identification number   ID_MAIN
  parentID      int8,       --  8   objID of parent object, = objID for parent/undeblended (ie. not child) object   ID_PARENT
  plate_ra      float8,     --  8 degrees Celestial Right Ascension   POS_EQ_RA_MAIN
  plate_dec     float8,     --  8 degrees Celestial Declination   POS_EQ_DEC_MAIN
  minx          float8,     --  8 microns Leftmost extent of image on plate   POS_PLATE_X
  maxx          float8,     --  8 microns Rightmost extent of image on plate    POS_PLATE_X
  miny          float8,     --  8 microns Bottommost extent of image on plate   POS_PLATE_Y
  maxy          float8,     --  8 microns Topmost extent of image on plate    POS_PLATE_Y
  area          int4,       --  4 pixels  Area within detection isophote    EXTENSION_AREA
  ipeak         float4,     --  4 instrumental units  Intensity of brightest pixel above sky    PHOT_INTENSITY_MISC
  cosmag        float4,     --  4 instrumental units  Isophotal magnitude   PHOT_PHG_MAG
  isky          float4,     --  4   Estimate of sky intensity at (xCen, yCen)   PHOT_SB_SKY
  xCen          float8,     --  8 microns Intensity-weighted X-centroid of image    POS_PLATE_X
  yCen          float8,     --  8 microns Intensity-weighted Y-centroid of image    POS_PLATE_Y
  aU            float4,     --  4 microns Semi-major axis of unweighted fitted ellipse    EXTENSION_RAD
  bU            float4,     --  4 microns Semi-minor axis of unweighted fitted ellipse    EXTENSION_SMIN
  thetaU        int2,       --  2 degrees Position angle (wrt x-axis) of unweighted fitted ellipse    POS_POS-ANG
  aI            float4,     --  4 microns Semi-major axis of intensity-weighted fitted ellip.   EXTENSION_RAD
  bI            float4,     --  4 microns Semi-minor axis of intensity-weighted fitted ellip.   EXTENSION_SMIN
  thetaI        int2,       --  2 degrees Position angle (wrt x-axis) of intensity-weighted fitted ellipse    POS_POS-ANG
  class         int2,       --  1     Classification flag   CLASS_OBJECT
  pa            int2,       --  2 degrees Celestial position angle (E of N)   POS_POS-ANG
  ap1           int4,       --  4 pixels  Area above 1st areal profile threshold    EXTENSION_AREA
  ap2           int4,       --  4 pixels  Area above 2nd areal profile threshold    EXTENSION_AREA
  ap3           int4,       --  4 pixels  Area above 3rd areal profile threshold    EXTENSION_AREA
  ap4           int4,       --  4 pixels  Area above 4th areal profile threshold    EXTENSION_AREA
  ap5           int4,       --  4 pixels  Area above 5th areal profile threshold    EXTENSION_AREA
  ap6           int4,       --  4 pixels  Area above 6th areal profile threshold    EXTENSION_AREA
  ap7           int4,       --  4 pixels  Area above 7th areal profile threshold    EXTENSION_AREA
  ap8           int4,       --  4 pixels  Area above 8th areal profile threshold    EXTENSION_AREA
  blend         int4,       --  4   Deblend flag: -n=parent of n children; +n = nth child of a parent   CODE_MULT_INDEX
  quality       int4,       --  4   Bitwise quality flag    CODE_QUALITY
  prfStat       float4,     --  4 N(0,1)  Profile statistic   CLASS_STAR/GALAXY
  prfMag        float4,     --  4 instrumental units  Profile magnitude   PHOT_PHG_MAG
  gMag          float4,     --  4 mag Magnitude calculated on assumption object is galaxy   MODEL_MAG_VALUE
  sMag          float4      --  4 mag Magnitude calculated on assumption object is star   MODEL_MAG_VALUE
) without oids;
