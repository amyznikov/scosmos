%
% load_targets( surveyid, plateid )  
%   Load scosmos target stars from scosmos binary file
%
%   Important Note:
%     The x and y are NOT reffered to plateid center X0, Y0, but to original plateid corner instead!!!
%
%

function [ objID xmin xmax ymin ymax area ipeak cosmag isky \
  xCen yCen aU bU thetaU aI bI thetaI class pa \
  ap1 ap2 ap3 ap4 ap5 ap6 ap7 ap8 \
  blend quality prfStat prfMag gMag sMag ] = load_targets( surveyid, plateid )

  %
  % Piped columns are:
  % 1  objID
  % 2  parentID
  % 3  ra
  % 4  dec
  % 5  xmin
  % 6  xmax
  % 7  ymin
  % 8  ymax
  % 9  area
  % 10 ipeak
  % 11 cosmag
  % 12 isky
  % 13 xCen
  % 14 yCen
  % 15 aU
  % 16 bU
  % 17 thetaU
  % 18 aI
  % 19 bI
  % 20 thetaI
  % 21 class
  % 22 pa
  % 23 ap1
  % 24 ap2
  % 25 ap3
  % 26 ap4
  % 27 ap5
  % 28 ap6
  % 29 ap7
  % 30 ap8
  % 31 blend
  % 32 quality
  % 33 prfStat
  % 34 prfMag
  % 35 gMag
  % 36 sMag
  %

  pipecmd = generate_plate_pipe_command( surveyid, plateid );
  if ( strcmp(pipecmd,"") )
    fprintf(stderr,"Can not locate plateid file for surveyid:%d plateid:%d\n", surveyid, plateid);
    return;
  end
  
  fid = popen(pipecmd, "r");
  if ( fid == -1 )
    fprintf(stderr,"popen('%s') fails\n", pipecmd);
    return;
  end
  
  v = fscanf(fid,'%lf',[36,Inf])';
  fclose(fid);

  objID       = v(:,1);
%  parentID    = v(:,2);
%  ra          = v(:,3);
%  dec         = v(:,4);
  xmin        = v(:,5);
  xmax        = v(:,6);
  ymin        = v(:,7);
  ymax        = v(:,8);
  area        = v(:,9);
  ipeak       = v(:,10);
  cosmag      = v(:,11);
  isky        = v(:,12);
  xCen        = v(:,13);
  yCen        = v(:,14);
  aU          = v(:,15);
  bU          = v(:,16);
  thetaU      = v(:,17);
  aI          = v(:,18);
  bI          = v(:,19);
  thetaI      = v(:,20);
  class       = v(:,21);
  pa          = v(:,22);
  ap1         = v(:,23);
  ap2         = v(:,24);
  ap3         = v(:,25);
  ap4         = v(:,26);
  ap5         = v(:,27);
  ap6         = v(:,28);
  ap7         = v(:,29);
  ap8         = v(:,30);
  blend       = v(:,31);
  quality     = v(:,32);
  prfStat     = v(:,33);
  prfMag      = v(:,34);
  gMag        = v(:,35);
  sMag        = v(:,36);
end



function pipecmd = generate_plate_pipe_command( surveyid, plateid )

  dataloc = get_ssa_data_location( surveyid );

  fname = sprintf("%s/%d-%d.clean.dat", dataloc, surveyid, plateid);
  if ( exist(fname,"file") )
    pipecmd = sprintf("ssa-plate-dump %s",fname)
    return;
  end
  
 
  fname = sprintf("%s/%d-%d.clean.dat.bz2", dataloc, surveyid, plateid);
  if ( exist(fname,"file") )
    pipecmd = sprintf("ssa-plate-dump -j %s",fname);
    return;
  end


  fname = sprintf("%s/%d-%d.dat", dataloc, surveyid, plateid);
  if ( exist(fname,"file") )
    pipecmd = sprintf("ssa-plate-dump -c %s",fname);
    return;
  end
  
  fname = sprintf("%s/%d-%d.dat.bz2", dataloc, surveyid, plateid);
  if ( exist(fname,"file") )
    pipecmd = sprintf("ssa-plate-dump -cj %s",fname);
    return;
  end
  
  pipecmd = "";
  
end


