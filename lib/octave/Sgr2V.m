%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   V(g,r) = g - 0.5784*(g - r) - 0.0038;  sigma = 0.0054
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%
function V = Sgr2V( g, r )
  V = g - 0.5784*(g - r) - 0.0038;
end
