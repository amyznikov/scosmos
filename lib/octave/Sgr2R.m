%
%  Transformations between SDSS magnitudes and UBVRcIc
%
%   R(g,r) = r - 0.1837*(g - r) - 0.0971;  sigma = 0.0106;
%   @see http://cas.sdss.org/dr6/en/help/docs/algorithm.asp?key=sdss2UBVRIT
%
function R = Sgr2R( g, r )
  R = r - 0.1837*(g - r) - 0.0971;
end
