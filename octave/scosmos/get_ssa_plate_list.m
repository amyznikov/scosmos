%
% platelist = get_plate_list(surveyid)
%   Query ssa_plates table and return the list of plate id's for given surveyid
%
%   Arguments:
%       surveyid: SSA survey number (0..9)
%
%       Example : 
%       platelist = get_ssa_plate_list(1);
%
function platelist = get_ssa_plate_list(surveyid)
  platelist = sscanf( pgquery("select plateid from ssa_plates where surveyid=%d", surveyid), "%d");
end
