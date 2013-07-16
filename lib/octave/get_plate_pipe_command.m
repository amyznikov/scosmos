%
% cmd = get_plate_pipe_command( plateid [, opts] )
%
function cmd = get_plate_pipe_command( plateid, opts )

  if ( nargin < 2 )
    opts = "";
  end

  path = ssa_data_location( plateid );
  if ( strcmp(path,"") )
    error("ssa_data_location() fails");
  end

  fname = sprintf('%s/%d.clean.dat', path, plateid);
  if ( exist(fname,'file') )
    cmd = sprintf('ssa-plate-dump %s %s -F', fname, opts);
    return;
  end

  fname = sprintf('%s/%d.clean.dat.bz2', path,  plateid);
  if ( exist(fname,'file') )
    cmd = sprintf('ssa-plate-dump %s %s -F', fname, opts);
    return;
  end

  fname = sprintf('%s/%d.dat', path,  plateid);
  if ( exist(fname,'file') )
    cmd = sprintf('ssa-plate-dump %s -c %s', fname, opts);
    return;
  end

  fname = sprintf('%s/%d.dat.bz2', path,  plateid);
  if ( exist(fname,'file') )
    cmd = sprintf('ssa-plate-dump %s -c %s', fname, opts);
    return;
  end

  cmd = '';

end
