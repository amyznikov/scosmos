%
% [l,b] = eq2gal( ra, dec )
%  
%  Convert equatorial coordinates to galactic.
%  All units are radians.
%
%  A.Myznikov, 
%   20 July 2013  
%
function [l,b] = eq2gal( ra, dec )

  RAGP  = 192.85948 * pi / 180;  % R.A. of NGP, J2000
  DEGP  = +27.12825 * pi / 180;  % Decl. of NGP, J2000
  LNGAN = +32.93192 * pi / 180;  % Galactic longitude of ascending node on equator

  sdegp = sin(DEGP);
  cdegp = cos(DEGP);
  
  sy = sin(dec);
  cy = cos(dec);
  sa = sin(ra-RAGP);
  ca = cos(ra-RAGP);
  sq = cy * cdegp * ca  + sy * sdegp;

  if ( nargout > 1 )
    b = asin(sq);
  end

  c = sy - sq * sdegp;
  d = cy * sa * cdegp;
  
  index = abs(d) < eps;
  d(index & d < 0 ) = -eps; 
  d(index & d >= 0 ) = +eps; 
  l = atan(c / d) + LNGAN;
  
  if ( any(index = d < 0) )
    l(index) += pi;
  end
  
  if ( any(index = d < 0) )
    l(index) += pi;
  end

  if ( any(index = l < 0) )
    l(index) += 2 * pi;
  end

  if ( any(index = l > 2 * pi) )
    l(index) -= 2 * pi;
  end

end
