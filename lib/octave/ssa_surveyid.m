% 
% surveyid = ssa_surveyid( plateid )
%   Return surveyid for given plateid
%
%   Arguments:
%       plateid: SSA plate id
%
function surveyid = ssa_surveyid( plateid )

  if ( plateid >= 65537 && plateid <= 66430 )
    surveyid = 1;
  elseif ( plateid >= 131073 && plateid <= 131966 )
    surveyid = 2;
	elseif ( plateid >= 196609 && plateid <= 197502 )
	  surveyid = 3;
	elseif ( plateid >= 262145 && plateid <= 262750 )
	  surveyid = 4;
	elseif ( plateid >= 327689 && plateid <= 329298 )
	  surveyid = 5;
	elseif ( plateid >= 393218 && plateid <= 394114 )
	  surveyid = 6;
	elseif ( plateid >= 458754 && plateid <= 459650 )
	  surveyid = 7;
	elseif ( plateid >= 524290 && plateid <= 525185 )
	  surveyid = 8;
	elseif ( plateid >= 589961 && plateid <= 591443 )
	  surveyid = 9;
  else
    surveyid = 0;
  end
  
end