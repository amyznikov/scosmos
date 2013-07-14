%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   I(i,z) = i - 0.3780*(i - z)  -0.3974;#  sigma = 0.0063
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%
function I = Siz2I( i, z)
  I = i - 0.3780*(i - z)  -0.3974;
end
