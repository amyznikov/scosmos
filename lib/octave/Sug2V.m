%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   V(u,g) = g - 0.2906*(u - g) + 0.0885;  sigma = 0.0129
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%

function V = Sug2V( u, g )
  V = g - 0.2906*(u - g) + 0.0885;
end
