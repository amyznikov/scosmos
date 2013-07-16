%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   B(u,g) = u - 0.8116*(u - g) + 0.1313;  sigma = 0.0095;
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%

function B = Sug2B( u, g )
  B = u - 0.8116*(u - g) + 0.1313;
end
