%
% [x, y, mag, xtan, ytan, ra, dec] = load_reference_stars( refname, surveyid, plateid [, minmag [, maxmag] ] )
%   Load prepared reference stars from refs/<surveyid>/<refname>/<plateid>.dat.
%   The x and y are refered to plate center X0, Y0
%
%   Usage:
%     [x, y, mag, xtan, ytan, ra, dec] = load_reference_stars( 'ucac4', 1, 66378 );
%
%

function [x, y, mag, xtan, ytan, ra, dec] = load_reference_stars( refname, surveyid, plateid, minmag, maxmag )

  x   = [];
  y   = [];
  mag = [];
  ra  = [];
  dec = [];
  xtan= [];
  ytan= [];

  
  % Use popenq() to grow flexibility and reduce memory pressure
  
  fname = sprintf('refs/%d/%s/%d.dat',surveyid, refname, plateid);
  fid = popenq(fname, 'ra,dec,x,y,smag' );
  if ( fid == -1 )
    fprintf(stderr, 'popenq(%s) fails\n', fname);
    return;
  end
  
  v  = fscanf(fid, '%lf', [5, Inf])';
  pclose(fid);

  if ( size(v) < 1 )
    fprintf(stderr, 'empty file %s \n', fname);
    return;
  end

  [A0, D0, ~, X0, Y0] = get_ssa_plate_info( surveyid, plateid );

  ra   = v(:,1);
  dec  = v(:,2);
  x    = v(:,3) - X0;
  y    = v(:,4) - Y0;
  mag  = v(:,5);


  if ( nargin > 3 )
    cond = mag >= minmag;
    if ( ~all(cond) )
      ra   = ra(cond);
      dec  = dec(cond);
      x    = x(cond);
      y    = y(cond);
      mag  = mag(cond);
    end
  end  


  if ( nargin > 4 )
    cond = mag <= minmag;
    if ( ~all(cond) )
      ra   = ra(cond);
      dec  = dec(cond);
      x    = x(cond);
      y    = y(cond);
      mag  = mag(cond);
    end
  end  


  if ( max(ra) - min(ra) > pi )
    cond = ra > pi;
    ra (cond) = ra (cond) - 2 * pi;
  end

  [xtan, ytan] = cs2tan(A0, D0, ra, dec);

end
