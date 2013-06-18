%
% [ra, dec] = tan2cs(A0, D0, xtan, ytan )
%   Convert tangential coordinates to celestial sphere
%
%   @See http://lambda.gsfc.nasa.gov/data/interfaces/iras/coordproj.html
%

function [ra, dec] = tan2cs(A0, D0, xtan, ytan )
 
  B = atan2(xtan, -ytan);
  D = atan( hypot(xtan,ytan) );

  ra  = atan2( xtan / cos(D0), 1.0 - ytan * tan(D0)) + A0;
  dec = asin( sin(D0) * cos(D) - cos(D0) * sin(D) .* cos(B));

  ra(ra >= 2 * pi) -= 2 * pi;
    
end
