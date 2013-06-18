%
% pipecmd = gen_plate_pipe_command( surveyid, plateid [, opts] )
%
function pipecmd = gen_plate_pipe_command( surveyid, plateid, opts )

  if ( nargin < 3 )
    opts = "";
  end

  path = get_ssa_data_location( surveyid );

  fname = sprintf('%s/%d-%d.clean.dat', path, surveyid, plateid);
  if ( exist(fname,'file') )
    pipecmd = sprintf('ssa-plate-dump %s %s -F', fname, opts);
    return;
  end


  fname = sprintf('%s/%d-%d.clean.dat.bz2', path, surveyid, plateid);
  if ( exist(fname,'file') )
    pipecmd = sprintf('ssa-plate-dump %s %s -F', fname, opts);
    return;
  end


  fname = sprintf('%s/%d-%d.dat', path, surveyid, plateid);
  if ( exist(fname,'file') )
    pipecmd = sprintf('ssa-plate-dump %s -c %s', fname, opts);
    return;
  end

  fname = sprintf('%s/%d-%d.dat.bz2', path, surveyid, plateid);
  if ( exist(fname,'file') )
    pipecmd = sprintf('ssa-plate-dump %s -c %s', fname, opts);
    return;
  end

  pipecmd = '';

end
