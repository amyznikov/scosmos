%
% [xmin, xmax, ymin, ymax ] = get_ssa_plate_bounds( surveyid )
%   Return the range of measured plate coordinates (microns), refered to plate center X0, Y0
%
%   Arguments:
%       surveyid : surveyid (0..9) from ssa_plates table
%
function [ xmin, xmax, ymin, ymax ] = get_ssa_plate_bounds( surveyid )

  A = sscanf(pgquery('select max(axmin),min(axmax),max(aymin),min(aymax) from ssa_plates where surveyid=%d', surveyid),'%lf');
  
  % Warning  : it seems that the columns are bugly reordered in original SSA database
  xmin = A(1);
  xmax = A(3);
  ymin = A(2);
  ymax = A(4);

  X0 = (xmin + xmax) / 2;
  Y0 = (ymin + ymax) / 2;
  
  xmin = xmin - X0;
  xmax = xmax - X0;
  ymin = ymin - Y0;
  ymax = ymax - Y0;
  
end

