%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   B(g,r) = g + 0.3130*(g - r) + 0.2271;  sigma = 0.0107
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%
function B = Sgr2B( g, r )
  B = g + 0.3130 * (g - r) + 0.2271;
end
