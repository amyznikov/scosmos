%
% prepare_reference_stars( plateid, refnames )
%   Write prepared reference stars into refs/<surveyid>/<refname>/<plateid>.dat.
%
%   Args:
%     plateid : SSA plateid 
%     refnames: Cell array of reference catalog names
%
function prepare_reference_stars( plateid, refnames )

  rtmp='/tmp/reference-stars.dat';
  stmp='/tmp/scosmos-stars.dat';
  ptmp='/tmp/pairs.dat';

  sid = ssa_surveyid(plateid);
  [A0, D0, EPOCH] = ssa_plate_info( plateid );

  for i = 1:size(refnames,2)

    refname = refnames{i};

    dirname = sprintf('refs/%d/%s', sid, refname );
    outname = sprintf('%s/%d.dat', dirname, plateid );
    if ( exist(outname,'file') )
      printf('%d looks processed as ''%s'' : skip it\n', plateid, outname );
      continue;
    end
    
    syscall('mkdir -p %s', dirname );
    
    if ( strcmp(refname,'ucac4') )
      selectCmd = generate_select_ucac4( A0, D0, EPOCH);
    else
      fprintf(stderr,'prepare_reference_stars() not implemented for "%s"\n', refname);
      continue;
    end

    cmd = get_plate_pipe_command( plateid, "-hf" );
    if ( strcmp(cmd, '' ) )
      fprintf(stderr,'get_plate_pipe_command() fails for plateid=%d\n', plateid);
      return;
    end

    cmd = sprintf('%s | cut -f 3,4,13,14,21,33-36 > %s', cmd, stmp);
    disp(cmd);
    syscall(cmd); 

    cmd = sprintf('psql wsdb -c \\\\"copy(%s) to stdout with csv header delimiter E''\\t'' null ''NA'' \\\\" > %s', selectCmd, rtmp);
    disp(cmd);
    syscall(cmd);

    cmd = sprintf('ssa-pair-stars -vd %s %s rc1=1 dc1=2 rc2=1 dc2=2 r=2.5 dups=drop > %s', rtmp, stmp, ptmp);
    disp(cmd);
    syscall(cmd); 

    if ( strcmp(refname,'ucac4') )
      cmd = sprintf('awk ''{if (FNR==1 || (\\\\$3==0 && \\\\$4==0)) { print \\\\$0; } }'' %s | cut --complement -f 3,4,5,6 > %s', ptmp, outname);
      disp(cmd);
      syscall(cmd);
    end

  end

end


function selectCmd = generate_select_ucac4( A0, D0, epoch)

  format = "select \
      (ra  + (%.3f-2000) * mura  * pi()/(1000.0 * 3600 * 180 * cos(dec))) as ra, \
      (dec + (%.3f-2000) * mudec * pi()/(1000.0 * 3600 * 180))  as dec, \
      cdf, objt \
  from ucac4 \
  where spoint(ra,dec) @ scircle( spoint(%15.9f,%15.9f), 6*pi()/180)"; 

  selectCmd = sprintf(format, epoch, epoch, A0, D0);

end
