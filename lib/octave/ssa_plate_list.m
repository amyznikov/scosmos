%
% platelist = ssa_plate_list(surveyid)
%   Query ssa_plates table and return the list of plate id's for given surveyid
%
%   Arguments:
%       surveyid: SSA survey number (1..9)
%
%   Example : 
%       platelist = ssa_plate_list(1);
%
function platelist = ssa_plate_list(surveyid)
  platelist = sscanf( pgquery('select plateid from ssa_plates where surveyid=%d', surveyid), '%d');
end
