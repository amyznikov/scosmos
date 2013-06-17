drop table if exists ssa_plates;
create table ssa_plates (
  plateID       int4,               -- 4     Unique identifier of the plate: surveyID plus field number combo
  surveyID      int2,               -- 1     Every plate comes from a survey
  fieldID       int4,               -- 4     Unique survey field identifier
  plateNum      int2,               -- 2     Plate number assigned by observatory
  dateMeas      varchar(10),        -- 8     MM-DD-YYYY    Measurement date    12-31-9999
  timeMeas      varchar(8),         -- 8     Measurement time (local), eg. 15:33:47
  instrument    varchar(16),        -- 16    Name of scanning machine
  softVersion   varchar(18),        -- 18    Software version, eg. /home/scosdev/v033
  operator      varchar(3),         -- 3     Machine operator's username
  scanMaterial  varchar(16),        -- 16    Photographic medium, eg. Original glass, Copy film, Copy glass
  emulsion      varchar(8),         -- 8     Kodak emulsion type, eg. 103a-E, IIIa-J, IV-N
  filterID      varchar(8),         -- 8     Filter identifier, eg. OG590, RG715, 2444...
  filterComm    varchar(16),        -- 16    Filter type, eg. "Schott glass", "Red plexiglass"...
  raPnt         float4,             -- 4     degrees    Mean plate centre RA
  decPnt        float4,             -- 4     degrees    Mean plate centre Dec
  radecSys      varchar(4),         -- 4     Co-ordinate system of raPnt/decPnt, eg. FK4
  equinox       float4,             -- 4     year    Equinox of raPnt and decPnt
  eqTsys        varchar(12),        -- 12    Time system of equinox, eg. BESSELIAN
  utDateObs     varchar(10),        -- 8     MM-DD-YYYY    UT date of observation    12-31-9999
  lstObs        varchar(4),         -- 4     24-hour    Local sidereal time at start of exposure, eg. 2336
  expLength     float4,             -- 4     minutes    Exposure time
  epoch         float4,             -- 4     years    Epoch of observation
  domeTemp      float4,             -- 4     Kelvin    Dome temp at observation
  domePressure  float4,             -- 4     mBar    Dome pressure at observation
  domeHumid     float4,             -- 4     Dome relative humidity at obsevation time
  waveEffect    int2,               -- 2     Angstrom    Effective wavelength
  tropl         float4,             -- 4     K/m    Tropospheric lapse rate
  mjd           float8,             -- 8     Julian days    Modified Julian Date at mid exposure
  focusNX       int2,               -- 1     No. of scan focus points in X
  focusNY       int2,               -- 1     No. of scan focus points in Y
  calType       varchar(16),        -- 16    Intensity calibration wedge fit type, eg. SPLINE
  stepWdg       varchar(16),        -- 16    Intensity calibration step-wedge type, eg. kpno
  nSteps        int2,               -- 1     No. of intensity calibration wedge steps
  orientation   varchar(4),         -- 4     SuperCOSMOS plate orientation, eg. news
  emulpos       varchar(4),         -- 4     SuperCOSMOS emulsion position, eg. up
  sosp          int2,               -- 2     SuperCOSMOS start of scan CCD pixel number
  stepsize      float4,             -- 4     microns    SuperCOSMOS nominal pixel size
  scanlen       int2,               -- 2     pixels    SuperCOSMOS CCD lane scan length
  aXmin         float4,             -- 4     microns    SuperCOSMOS XMIN of measured area
  aXmax         float4,             -- 4     microns    SuperCOSMOS XMAX of measured area
  aYmin         float4,             -- 4     microns    SuperCOSMOS YMIN of measured area
  aYmax         float4,             -- 4     microns    SuperCOSMOS YMAX of measured area
  xPnt          float4,             -- 4     microns    Centre of scan area in X
  yPnt          float4,             -- 4     microns    Centre of scan area in Y
  areaCut       int2,               -- 1     pixels    Minimum area of detected object
  apParam       float4,             -- 4     Areal profile parameter
  dbParam       float4,             -- 4     Deblending parameter
  dbAmin        int2,               -- 1     pixels    Minimum area of parent image that can be deblended
  dbAmax        int4,               -- 4     pixels    Maximum area of parent image that can be deblended
  dbAcut        int2,               -- 1     pixels    Minimum area of deblended object detection
  dbLevel       int2,               -- 1     No. of rethresholding levels for deblending
  skySquare     int2,               -- 2     pixels    Blocksize (one side) of sky background
  skyDefn       varchar(16),        -- 16    Method of defining sky, eg. "median"
  skyFilter     varchar(16),        -- 16    Box filter weights for sky determination
  fThresh       int2,               -- 1     Sky filtering parameter
  fSclen        int2,               -- 1     Scale size for iterative filter application
  pCut          float4,             -- 4     percent    Threshold "percentage cut" for pixel analysis
  starCat       varchar(80),        -- 80    Name of reference star catalogue used for astrometric fit
  brightLim     float4,             -- 4     Bright mag limit for selected astrometric refstars
  faintLim      float4,             -- 4     Faint limit for selected refstars
  equinRef      float4,             -- 4     Equinox of refstar positions
  tSysRef       varchar(12),        -- 12    Time system of equinox of refstar positions, eg. JULIAN
  maxIter       int4,               -- 4     Maximum no. of iterations for non-linear astrometric fit
  rCritIni      float4,             -- 4     microns    Initial matching tolerance for refstars
  rCritAbs      float4,             -- 4     microns    Absolute object rejection criterion
  rCritRel      float4,             -- 4     Relative object rejection criterion
  rCritFin      float4,             -- 4     sigma    Final residual tolerance
  distType      varchar(80),        -- 80    Radial distortion term in solution, eg. R/TAN(R)
  scdGrid       varchar(80),        -- 80    Non-linear distortion mask filename
  raCol         varchar(16),        -- 16    Name of RA column in reference catalogue
  decCol        varchar(16),        -- 16    Name of Dec column in reference catalogue
  raPMCol       varchar(16),        -- 16    Name of RA proper motion column in reference catalogue
  decPMCol      varchar(16),        -- 16    Name of Dec proper motion column in reference catalogue
  plxCol        varchar(16),        -- 16    Name of parallax column in reference catalogue
  rvCol         varchar(16),        -- 16    Name of radial velocity column in reference catalogue
  magCol        varchar(16),        -- 16    Name of magnitude column in reference catalogue
  cEquinox      float4,             -- 4     year    Catalogue equinox
  cEqTSys       varchar(16),        -- 16    Catalogue equinox time system, eg. JULIAN
  cEpoch        float4,             -- 4     year    Catalogue epoch
  cEpTSys       varchar(16),        -- 16    Catalogue epoch time system, eg. JULIAN
  starsInit     int4,               -- 4     No. of refstars in initial astrometric solution
  starsSelec    int4,               -- 4     No. of refstars initially selected
  starsUsed     int4,               -- 4     No. of refstars in final astrometric solution
  coeffs1       float8,             -- 8     1st coefficient of astrometric fit for XY to obs. xi,eta
  coeffs2       float8,             -- 8     2nd coefficient of astrometric fit for XY tos. xi,eta
  coeffs3       float8,             -- 8     3rd coefficient of astrometric fit for XY to obs. xi,eta
  coeffs4       float8,             -- 8     4th coefficient of astrometric fit for XY to obs. xi,eta
  coeffs5       float8,             -- 8     5th coefficient of astrometric fit for XY to obs. xi,eta
  coeffs6       float8,             -- 8     6th coefficient of astrometric fit for XY to obs. xi,eta
  cubicDist     float8,             -- 8     Cubic coefficient of radial distortion
  obsRaPnt      float8,             -- 8     radians    Observed plate centre RA
  obsDecPnt     float8,             -- 8     radians    Observed plate centre Dec
  shiftPnt      float8,             -- 8     radians    Shift of from nominal to fitted plate centre
  areaMin       int4,               -- 4     pixels    Minimum object area in IAM file
  areaMax       int4,               -- 4     pixels    Maximum object area in IAM file
  magMin        float4,             -- 4     Minimum instrumental mag in IAM file
  magMax        float4,             -- 4     Maximum instrumental mag in IAM file
  ellipMin      float4,             -- 4     Minimum object ellipticity in IAM file
  ellipMax      float4,             -- 4     Maximum object ellipticity in IAM file
  ellipMode     float4,             -- 4     Modal object ellipticity in IAM file
  orientMode    float4,             -- 4     degrees    Modal orientation in IAM file
  ellipMed      float4,             -- 4     Median ellipticity in IAM file
  orientMed     float4,             -- 4     degrees    Median orientation in IAM file
  ellipMean     float4,             -- 4     Mean object ellipticity in IAM file
  orientMean    float4,             -- 4     degrees    Mean orientation in IAM file
  objNum        int4,               -- 4     No. of objects detected on plate
  nParents      int4,               -- 4     No. of parent objects detected on plate
  lane1Count    int4,               -- 4     No. of objects in lane 1 of plate scan
  lane2Count    int4,               -- 4     No. of objects in lane 2 of plate scan
  lane3Count    int4,               -- 4     No. of objects in lane 3 of plate scan
  lane4Count    int4,               -- 4     No. of objects in lane 4 of plate scan
  lane5Count    int4,               -- 4     No. of objects in lane 5 of plate scan
  lane6Count    int4,               -- 4     No. of objects in lane 6 of plate scan
  lane7Count    int4,               -- 4     No. of objects in lane 7 of plate scan
  lane8Count    int4,               -- 4     No. of objects in lane 8 of plate scan
  lane9Count    int4,               -- 4     No. of objects in lane 9 of plate scan
  lane10Count   int4,               -- 4     No. of objects in lane 10 of plate scan
  lane11Count   int4,               -- 4     No. of objects in lane 11 of plate scan
  lane12Count   int4,               -- 4     No. of objects in lane 12 of plate scan
  lane13Count   int4,               -- 4     No. of objects in lane 13 of plate scan
  lane14Count   int4,               -- 4     No. of objects in lane 14 of plate scan
  lane15Count   int4,               -- 4     No. of objects in lane 15 of plate scan
  lane16Count   int4,               -- 4     No. of objects in lane 16 of plate scan
  lane17Count   int4,               -- 4     No. of objects in lane 17 of plate scan
  lane18Count   int4,               -- 4     No. of objects in lane 18 of plate scan
  lane19Count   int4,               -- 4     No. of objects in lane 19 of plate scan
  lane20Count   int4,               -- 4     No. of objects in lane 20 of plate scan
  lane21Count   int4,               -- 4     No. of objects in lane 21 of plate scan
  lane22Count   int4,               -- 4     No. of objects in lane 22 of plate scan
  lane23Count   int4,               -- 4     No. of objects in lane 23 of plate scan
  lane24Count   int4,               -- 4     No. of objects in lane 24 of plate scan
  lane25Count   int4,               -- 4     No. of objects in lane 25 of plate scan
  lane26Count   int4,               -- 4     No. of objects in lane 26 of plate scan
  lane27Count   int4,               -- 4     No. of objects in lane 27 of plate scan
  lane28Count   int4,               -- 4     No. of objects in lane 28 of plate scan
  lane29Count   int4,               -- 4     No. of objects in lane 29 of plate scan
  lane30Count   int4,               -- 4     No. of objects in lane 30 of plate scan
  astResidX     float4,             -- 4     arcsec    Residual of astrometric fit in X
  astResidY     float4,             -- 4     arcsec    Residual of astrometric fit in Y
  nCalCoeffs    int2,               -- 1     No. of photometric calibration coefficients
  CalXK         float4,             -- 4     inst. profile mag    Stationary point in profile cal. curve
  gradient      float4,             -- 4     Gradient of linear portion of profile cal. curve
  getCal1       float8,             -- 8     1st profile calibration polynomial coefficient
  getCal2       float8,             -- 8     2nd profile calibration polynomial coefficient
  getCal3       float8,             -- 8     3rd profile calibration polynomial coefficient
  getCal4       float8,             -- 8     4th profile calibration polynomial coefficient
  getCal5       float8,             -- 8     5th profile calibration polynomial coefficient
  getCal6       float8,             -- 8     6th profile calibration polynomial coefficient
  ffZp          float4,             -- 4     mag    Field-to-field zeropoint
  galZp         float8,             -- 8     mag    Zeropoint of galaxy instrumental scale
  galGrad       float8,             -- 8     mags per instrumental mag    Gradient of galaxy instrumental mag scale
  directory     varchar(80)         -- 80    Name of directory in flat-file system of source data.
) without oids;



COMMENT ON TABLE ssa_plates is 'See http://surveys.roe.ac.uk/ssa/www/SSA_TABLE_PlateSchema.html#Plate';

COMMENT ON COLUMN ssa_plates.plateID        is 'Unique identifier of the plate: surveyID plus field number combo';
COMMENT ON COLUMN ssa_plates.surveyID       is 'Every plate comes from a survey';
COMMENT ON COLUMN ssa_plates.fieldID        is 'Unique survey field identifier';
COMMENT ON COLUMN ssa_plates.plateNum       is 'Plate number assigned by observatory';
COMMENT ON COLUMN ssa_plates.dateMeas       is 'MM-DD-YYYY    Measurement date    12-31-9999';
COMMENT ON COLUMN ssa_plates.timeMeas       is 'Measurement time (local), eg. 15:33:47';
COMMENT ON COLUMN ssa_plates.instrument     is 'Name of scanning machine';
COMMENT ON COLUMN ssa_plates.softVersion    is 'Software version, eg. /home/scosdev/v033';
COMMENT ON COLUMN ssa_plates.operator       is 'Machine operator username';
COMMENT ON COLUMN ssa_plates.scanMaterial   is 'Photographic medium, eg. Original glass, Copy film, Copy glass';
COMMENT ON COLUMN ssa_plates.emulsion       is 'Kodak emulsion type, eg. 103a-E, IIIa-J, IV-N';
COMMENT ON COLUMN ssa_plates.filterID       is 'Filter identifier, eg. OG590, RG715, 2444.';
COMMENT ON COLUMN ssa_plates.filterComm     is 'Filter type, eg. "Schott glass", "Red plexiglass"';
COMMENT ON COLUMN ssa_plates.raPnt          is 'degrees    Mean plate centre RA';
COMMENT ON COLUMN ssa_plates.decPnt         is 'degrees    Mean plate centre Dec';
COMMENT ON COLUMN ssa_plates.radecSys       is 'Co-ordinate system of raPnt/decPnt, eg. FK4';
COMMENT ON COLUMN ssa_plates.equinox        is 'year    Equinox of raPnt and decPnt';
COMMENT ON COLUMN ssa_plates.eqTsys         is 'Time system of equinox, eg. BESSELIAN';
COMMENT ON COLUMN ssa_plates.utDateObs      is 'MM-DD-YYYY    UT date of observation    12-31-9999    TIME_DATE';
COMMENT ON COLUMN ssa_plates.lstObs         is '24-hour    Local sidereal time at start of exposure, eg. 2336';
COMMENT ON COLUMN ssa_plates.expLength      is 'minutes    Exposure time';
COMMENT ON COLUMN ssa_plates.epoch          is 'years    Epoch of observation';
COMMENT ON COLUMN ssa_plates.domeTemp       is 'Kelvin    Dome temp at observation';
COMMENT ON COLUMN ssa_plates.domePressure   is 'mBar    Dome pressure at observation';
COMMENT ON COLUMN ssa_plates.domeHumid      is 'Dome relative humidity at obsevation time';
COMMENT ON COLUMN ssa_plates.waveEffect     is 'Angstrom    Effective wavelength';
COMMENT ON COLUMN ssa_plates.tropl          is 'K/m    Tropospheric lapse rate';
COMMENT ON COLUMN ssa_plates.mjd            is 'Julian days    Modified Julian Date at mid exposure';
COMMENT ON COLUMN ssa_plates.focusNX        is 'No. of scan focus points in X';
COMMENT ON COLUMN ssa_plates.focusNY        is 'No. of scan focus points in Y';
COMMENT ON COLUMN ssa_plates.calType        is 'Intensity calibration wedge fit type, eg. SPLINE';
COMMENT ON COLUMN ssa_plates.stepWdg        is 'Intensity calibration step-wedge type, eg. kpno';
COMMENT ON COLUMN ssa_plates.nSteps         is 'No. of intensity calibration wedge steps';
COMMENT ON COLUMN ssa_plates.orientation    is 'SuperCOSMOS plate orientation, eg. news';
COMMENT ON COLUMN ssa_plates.emulpos        is 'SuperCOSMOS emulsion position, eg. up';
COMMENT ON COLUMN ssa_plates.sosp           is 'SuperCOSMOS start of scan CCD pixel number';
COMMENT ON COLUMN ssa_plates.stepsize       is 'microns    SuperCOSMOS nominal pixel size';
COMMENT ON COLUMN ssa_plates.scanlen        is 'pixels    SuperCOSMOS CCD lane scan length';
COMMENT ON COLUMN ssa_plates.aXmin          is 'microns    SuperCOSMOS XMIN of measured area';
COMMENT ON COLUMN ssa_plates.aXmax          is 'microns    SuperCOSMOS XMAX of measured area';
COMMENT ON COLUMN ssa_plates.aYmin          is 'microns    SuperCOSMOS YMIN of measured area';
COMMENT ON COLUMN ssa_plates.aYmax          is 'microns    SuperCOSMOS YMAX of measured area';
COMMENT ON COLUMN ssa_plates.xPnt           is 'microns    Centre of scan area in X';
COMMENT ON COLUMN ssa_plates.yPnt           is 'microns    Centre of scan area in Y';
COMMENT ON COLUMN ssa_plates.areaCut        is 'pixels    Minimum area of detected object';
COMMENT ON COLUMN ssa_plates.apParam        is 'Areal profile parameter';
COMMENT ON COLUMN ssa_plates.dbParam        is 'Deblending parameter';
COMMENT ON COLUMN ssa_plates.dbAmin         is 'pixels    Minimum area of parent image that can be deblended';
COMMENT ON COLUMN ssa_plates.dbAmax         is 'pixels    Maximum area of parent image that can be deblended';
COMMENT ON COLUMN ssa_plates.dbAcut         is 'pixels    Minimum area of deblended object detection';
COMMENT ON COLUMN ssa_plates.dbLevel        is 'No. of rethresholding levels for deblending';
COMMENT ON COLUMN ssa_plates.skySquare      is 'pixels    Blocksize (one side) of sky background';
COMMENT ON COLUMN ssa_plates.skyDefn        is 'Method of defining sky, eg. "median"';
COMMENT ON COLUMN ssa_plates.skyFilter      is 'Box filter weights for sky determination';
COMMENT ON COLUMN ssa_plates.fThresh        is 'Sky filtering parameter';
COMMENT ON COLUMN ssa_plates.fSclen         is 'Scale size for iterative filter application';
COMMENT ON COLUMN ssa_plates.pCut           is 'percent    Threshold "percentage cut" for pixel analysis         STAT_%REJECT';
COMMENT ON COLUMN ssa_plates.starCat        is 'Name of reference star catalogue used for astrometric fit';
COMMENT ON COLUMN ssa_plates.brightLim      is 'Bright mag limit for selected astrometric refstars';
COMMENT ON COLUMN ssa_plates.faintLim       is 'Faint limit for selected refstars';
COMMENT ON COLUMN ssa_plates.equinRef       is 'Equinox of refstar positions';
COMMENT ON COLUMN ssa_plates.tSysRef        is 'Time system of equinox of refstar positions, eg. JULIAN';
COMMENT ON COLUMN ssa_plates.maxIter        is 'Maximum no. of iterations for non-linear astrometric fit';
COMMENT ON COLUMN ssa_plates.rCritIni       is 'microns    Initial matching tolerance for refstars';
COMMENT ON COLUMN ssa_plates.rCritAbs       is 'microns    Absolute object rejection criterion';
COMMENT ON COLUMN ssa_plates.rCritRel       is 'Relative object rejection criterion';
COMMENT ON COLUMN ssa_plates.rCritFin       is 'sigma    Final residual tolerance';
COMMENT ON COLUMN ssa_plates.distType       is 'Radial distortion term in solution, eg. R/TAN(R)';
COMMENT ON COLUMN ssa_plates.scdGrid        is 'Non-linear distortion mask filename';
COMMENT ON COLUMN ssa_plates.raCol          is 'Name of RA column in reference catalogue';
COMMENT ON COLUMN ssa_plates.decCol         is 'Name of Dec column in reference catalogue';
COMMENT ON COLUMN ssa_plates.raPMCol        is 'Name of RA proper motion column in reference catalogue';
COMMENT ON COLUMN ssa_plates.decPMCol       is 'Name of Dec proper motion column in reference catalogue';
COMMENT ON COLUMN ssa_plates.plxCol         is 'Name of parallax column in reference catalogue';
COMMENT ON COLUMN ssa_plates.rvCol          is 'Name of radial velocity column in reference catalogue';
COMMENT ON COLUMN ssa_plates.magCol         is 'Name of magnitude column in reference catalogue';
COMMENT ON COLUMN ssa_plates.cEquinox       is 'year  Catalogue equinox';
COMMENT ON COLUMN ssa_plates.cEqTSys        is 'Catalogue equinox time system, eg. JULIAN';
COMMENT ON COLUMN ssa_plates.cEpoch         is 'year  Catalogue epoch';
COMMENT ON COLUMN ssa_plates.cEpTSys        is 'Catalogue epoch time system, eg. JULIAN';
COMMENT ON COLUMN ssa_plates.starsInit      is 'No. of refstars in initial astrometric solution';
COMMENT ON COLUMN ssa_plates.starsSelec     is 'No. of refstars initially selected';
COMMENT ON COLUMN ssa_plates.starsUsed      is 'No. of refstars in final astrometric solution';
COMMENT ON COLUMN ssa_plates.coeffs1        is '1st coefficient of astrometric fit for XY to obs. xi,eta';
COMMENT ON COLUMN ssa_plates.coeffs2        is '2nd coefficient of astrometric fit for XY to obs. xi,eta';
COMMENT ON COLUMN ssa_plates.coeffs3        is '3rd coefficient of astrometric fit for XY to obs. xi,eta';
COMMENT ON COLUMN ssa_plates.coeffs4        is '4th coefficient of astrometric fit for XY to obs. xi,eta';
COMMENT ON COLUMN ssa_plates.coeffs5        is '5th coefficient of astrometric fit for XY to obs. xi,eta';
COMMENT ON COLUMN ssa_plates.coeffs6        is '6th coefficient of astrometric fit for XY to obs. xi,eta';
COMMENT ON COLUMN ssa_plates.cubicDist      is 'Cubic coefficient of radial distortion';
COMMENT ON COLUMN ssa_plates.obsRaPnt       is 'radians    Observed plate centre RA';
COMMENT ON COLUMN ssa_plates.obsDecPnt      is 'radians    Observed plate centre Dec';
COMMENT ON COLUMN ssa_plates.shiftPnt       is 'radians    Shift of from nominal to fitted plate centre';
COMMENT ON COLUMN ssa_plates.areaMin        is 'pixels    Minimum object area in IAM file';
COMMENT ON COLUMN ssa_plates.areaMax        is 'pixels    Maximum object area in IAM file';
COMMENT ON COLUMN ssa_plates.magMin         is 'Minimum instrumental mag in IAM file';
COMMENT ON COLUMN ssa_plates.magMax         is 'Maximum instrumental mag in IAM file';
COMMENT ON COLUMN ssa_plates.ellipMin       is 'Minimum object ellipticity in IAM file';
COMMENT ON COLUMN ssa_plates.ellipMax       is 'Maximum object ellipticity in IAM file';
COMMENT ON COLUMN ssa_plates.ellipMode      is 'Modal object ellipticity in IAM file';
COMMENT ON COLUMN ssa_plates.orientMode     is 'degrees    Modal orientation in IAM file';
COMMENT ON COLUMN ssa_plates.ellipMed       is 'Median ellipticity in IAM file';
COMMENT ON COLUMN ssa_plates.orientMed      is 'degrees    Median orientation in IAM file';
COMMENT ON COLUMN ssa_plates.ellipMean      is 'Mean object ellipticity in IAM file';
COMMENT ON COLUMN ssa_plates.orientMean     is 'degrees    Mean orientation in IAM file';
COMMENT ON COLUMN ssa_plates.objNum         is 'No. of objects detected on plate';
COMMENT ON COLUMN ssa_plates.nParents       is 'No. of parent objects detected on plate';
COMMENT ON COLUMN ssa_plates.lane1Count     is 'No. of objects in lane 1 of plate scan';
COMMENT ON COLUMN ssa_plates.lane2Count     is 'No. of objects in lane 2 of plate scan';
COMMENT ON COLUMN ssa_plates.lane3Count     is 'No. of objects in lane 3 of plate scan';
COMMENT ON COLUMN ssa_plates.lane4Count     is 'No. of objects in lane 4 of plate scan';
COMMENT ON COLUMN ssa_plates.lane5Count     is 'No. of objects in lane 5 of plate scan';
COMMENT ON COLUMN ssa_plates.lane6Count     is 'No. of objects in lane 6 of plate scan';
COMMENT ON COLUMN ssa_plates.lane7Count     is 'No. of objects in lane 7 of plate scan';
COMMENT ON COLUMN ssa_plates.lane8Count     is 'No. of objects in lane 8 of plate scan';
COMMENT ON COLUMN ssa_plates.lane9Count     is 'No. of objects in lane 9 of plate scan';
COMMENT ON COLUMN ssa_plates.lane10Count    is 'No. of objects in lane 10 of plate scan';
COMMENT ON COLUMN ssa_plates.lane11Count    is 'No. of objects in lane 11 of plate scan';
COMMENT ON COLUMN ssa_plates.lane12Count    is 'No. of objects in lane 12 of plate scan';
COMMENT ON COLUMN ssa_plates.lane13Count    is 'No. of objects in lane 13 of plate scan';
COMMENT ON COLUMN ssa_plates.lane14Count    is 'No. of objects in lane 14 of plate scan';
COMMENT ON COLUMN ssa_plates.lane15Count    is 'No. of objects in lane 15 of plate scan';
COMMENT ON COLUMN ssa_plates.lane16Count    is 'No. of objects in lane 16 of plate scan';
COMMENT ON COLUMN ssa_plates.lane17Count    is 'No. of objects in lane 17 of plate scan';
COMMENT ON COLUMN ssa_plates.lane18Count    is 'No. of objects in lane 18 of plate scan';
COMMENT ON COLUMN ssa_plates.lane19Count    is 'No. of objects in lane 19 of plate scan';
COMMENT ON COLUMN ssa_plates.lane20Count    is 'No. of objects in lane 20 of plate scan';
COMMENT ON COLUMN ssa_plates.lane21Count    is 'No. of objects in lane 21 of plate scan';
COMMENT ON COLUMN ssa_plates.lane22Count    is 'No. of objects in lane 22 of plate scan';
COMMENT ON COLUMN ssa_plates.lane23Count    is 'No. of objects in lane 23 of plate scan';
COMMENT ON COLUMN ssa_plates.lane24Count    is 'No. of objects in lane 24 of plate scan';
COMMENT ON COLUMN ssa_plates.lane25Count    is 'No. of objects in lane 25 of plate scan';
COMMENT ON COLUMN ssa_plates.lane26Count    is 'No. of objects in lane 26 of plate scan';
COMMENT ON COLUMN ssa_plates.lane27Count    is 'No. of objects in lane 27 of plate scan';
COMMENT ON COLUMN ssa_plates.lane28Count    is 'No. of objects in lane 28 of plate scan';
COMMENT ON COLUMN ssa_plates.lane29Count    is 'No. of objects in lane 29 of plate scan';
COMMENT ON COLUMN ssa_plates.lane30Count    is 'No. of objects in lane 30 of plate scan';
COMMENT ON COLUMN ssa_plates.astResidX      is 'arcsec    Residual of astrometric fit in X';
COMMENT ON COLUMN ssa_plates.astResidY      is 'arcsec    Residual of astrometric fit in Y';
COMMENT ON COLUMN ssa_plates.nCalCoeffs     is 'No. of photometric calibration coefficients';
COMMENT ON COLUMN ssa_plates.CalXK          is 'inst. profile mag    Stationary point in profile cal. curve';
COMMENT ON COLUMN ssa_plates.gradient       is 'Gradient of linear portion of profile cal. curve';
COMMENT ON COLUMN ssa_plates.getCal1        is '1st profile calibration polynomial coefficient';
COMMENT ON COLUMN ssa_plates.getCal2        is '2nd profile calibration polynomial coefficient';
COMMENT ON COLUMN ssa_plates.getCal3        is '3rd profile calibration polynomial coefficient';
COMMENT ON COLUMN ssa_plates.getCal4        is '4th profile calibration polynomial coefficient';
COMMENT ON COLUMN ssa_plates.getCal5        is '5th profile calibration polynomial coefficient';
COMMENT ON COLUMN ssa_plates.getCal6        is '6th profile calibration polynomial coefficient';
COMMENT ON COLUMN ssa_plates.ffZp           is 'mag    Field-to-field zeropoint';
COMMENT ON COLUMN ssa_plates.galZp          is 'mag    Zeropoint of galaxy instrumental scale';
COMMENT ON COLUMN ssa_plates.galGrad        is 'mags per instrumental mag  Gradient of galaxy instrumental mag scale';
COMMENT ON COLUMN ssa_plates.directory      is 'Name of directory in flat-file system of source data';
