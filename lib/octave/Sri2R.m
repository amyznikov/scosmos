%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   R(r,i) = r - 0.2936*(r - i) - 0.1439;  sigma = 0.0072;
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%
function R = Sri2R( r, i )
  R = r - 0.2936*(r - i) - 0.1439;
end

