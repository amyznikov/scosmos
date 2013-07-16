%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   I(r,i) = r - 1.2444*(r - i) - 0.3820;  sigma = 0.0078
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%
function I = Sri2I( r, i )
  I = r - 1.2444*(r - i) - 0.3820;
end
