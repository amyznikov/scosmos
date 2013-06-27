%
% [x, y, mag, xtan, ytan, ra, dec] = load_reference_stars( refname, plateid [, minmag [, maxmag] ] )
%   Load prepared reference stars from refs/<surveyid>/<refname>/<plateid>.dat.
%   The x and y are refered to plate center X0, Y0
%
%   Usage:
%     [x, y, mag, xtan, ytan, ra, dec] = load_reference_stars( 'ucac4', 66378 );
%
%

function [x, y, mag, xtan, ytan, ra, dec] = load_reference_stars( refname, plateid, minmag, maxmag )

  x   = [];
  y   = [];
  mag = [];
  ra  = [];
  dec = [];
  xtan= [];
  ytan= [];


  % Use popenq() for more flexibility and reduce memory pressure

  fname = sprintf('refs/%d/%s/%d.dat', ssa_surveyid(plateid), refname, plateid);
  fid = popenq(fname, 'ra,dec,x,y,sMag' );
  if ( fid == -1 )
    fprintf(stderr, 'popenq(%s) fails\n', fname);
    return;
  end

  v  = fscanf(fid, '%lf', [5, Inf])';
  pclose(fid);

  if ( size(v,1) < 1 || size(v,2) < 1 )
    fprintf(stderr, '%s: empty file\n', fname);
    return;
  end

  [A0, D0, ~, X0, Y0] = ssa_plate_info( plateid );

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
