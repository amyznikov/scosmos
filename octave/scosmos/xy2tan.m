%
% [mx, my] = xy2tan( x, y )
%   Default scosmos plate model to convert measured (x,y) plate coordinates to tangential plane (xtan,ytan)
%
%  Usage:
%     % Generate model using reference stars:
%      [mx, my] = xy2tan(xref, yref);
%
%     % solve model for reference stars:
%      [coeffx, sigmax] = ols(xtanref, mx );
%      [coeffy, sigmay] = ols(ytanref, my );
%
%     % Generate model for target stars:
%      [mx, my] = xy2tan(x, y);
%
%     % Apply model to target stars:
%       xtan = mx * coeffx;
%       ytan = my * coeffy;
%
%     % Convert targets tangential plane to celestial sphere:
%       [ra,dec] = tan2cs(A0,D0,xtan,ytan);
%

function [mx, my] = xy2tan( x, y )
  
  mx = [ \
      ones(size(x))        y         y.^2           y.^3              \
      x                 x.*y      x.*y.^2                    x.*y.^4  \
      x.^2                                                            \
      x.^3                     x.^3.*y.^2                             \
                                                                      \
    ];
  
  my = [ \
      ones(size(x))        y         y.^2           y.^3              \
      x                 x.*y                                          \
      x.^2           x.^2.*y                  x.^2.*y.^3              \
      x.^3                                                            \
                     x.^4.*y                                          \
    ];


end

