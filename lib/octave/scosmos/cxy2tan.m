%
% [coeffx, coeffy, sigmax, sigmay, dx, dy] = cxy2tan( K, x, y, xtan, ytan [, fn, ...])
%   Make reduction of measured (x,y) coordinates to tangential plane (xtan,ytan) 
%   using optional model generator fn and K-sigma junk threshold
%
%   Example:
%     [coeffx, coeffy, sigmax, sigmay] = cxy2tan( 3, x, y, xtan, ytan, @xy2tan )
%

function [coeffx, coeffy, sigmax, sigmay] = cxy2tan( K, x, y, xtan, ytan, fn, varargin )

  % 2-step reduction removing bad pairs after first step
  
  for pass = 1:2
    
    % generate model
    if ( nargin < 6 )
      [mx, my] = xy2tan(x, y);
    else
      [mx, my] = fn(x, y, varargin{:});
    end
    
    % solve model
    [coeffx, sigmax, dx] = ols(xtan, mx );
    [coeffy, sigmay, dy] = ols(ytan, my );

    % check if bad stars must be rejected
    if ( K <= 0 || pass > 1 )
      break;
    end
    
    % remove bad pairs with residuals more than K * sigma
    below_k_sigma = abs(dx) < K * sqrt(sigmax) & abs(dy) < K * sqrt(sigmay);
    if ( all(below_k_sigma) )
      break;
    end

    x    = x(below_k_sigma);
    y    = y(below_k_sigma);
    xtan = xtan(below_k_sigma);
    ytan = ytan(below_k_sigma);
  end

end
