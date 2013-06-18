%
% [xtan, ytan] = cs2tan( A0, D0, ra, dec)
%   Convert celestial sphere coordinates to tangential plane
%
%   @See http://lambda.gsfc.nasa.gov/data/interfaces/iras/coordproj.html
%
function [xtan, ytan] = cs2tan( A0, D0, ra, dec)
  A = cos(dec) .* cos(ra - A0);
  den = (sin(dec) * sin(D0) + A * cos(D0) );

  xtan = cos(dec) .* sin(ra - A0) ./ den;
  ytan = (cos(D0) * sin(dec) - A * sin(D0)) ./ den;
end
