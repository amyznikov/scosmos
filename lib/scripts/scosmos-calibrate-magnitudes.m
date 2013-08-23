#! /usr/bin/octave -qf

format short;
ignore_function_time_stamp("all");
if ( exist(__sub=sprintf("%s/octave",pwd()),'dir' ) )
  addpath(__sub);
end

pkg load optim;


function show_usage( stream )
  fprintf(stream, 'USAGE:\n');
  fprintf(stream, '  %s [-o {output-name}] [-K K] [-N NPASS] [-A] magrefs.dat ...\n', program_name());
end



function [cosmag, isky, x, y, el, Bc, C,ra,dec] = loadmagrefs( filename )

  columns = 'cosmag,isky,x,y,el:(1.0-@bI/@aI),Bc,C,ra,dec';
  cond = '@bI/@aI>0.5 && (@class==2 || @C==1)';

  cmd = sprintf('ccut -f \"%s\" -x \"%s\" %s', columns, cond, filename);
  fid = popen(cmd,'r');
  if ( fid == -1 )
    fprintf(stderr,'Error: can not read %s\n',filename);
    return;
  end

  fgets(fid);
  v = transpose(fscanf(fid,'%lf',[9,Inf])); 
  pclose(fid);

  % remove bad isky
  smin1 = min( v(:,2) ) + 1e6;
  smax1 = max( v(:,2) ) - 1e6;
  v = v(v(:,2) >= smin1 & v(:,2) <= smax1,:);

  cosmag = v(:,1);
  isky   = v(:,2)*1e-7 - 2.5;
  x      = v(:,3) - 175000;
  y      = v(:,4) - 177500;
  el     = v(:,5);
  Bc     = v(:,6);
  C      = v(:,7);
  ra     = v(:,8);
  dec    = v(:,9);


end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%
% Model is:
%  bc = a0+a1/(exp(a2*(cosmag-a4))+exp(a3*(cosmag-a4))+1) + 
%        Pn(isky) + Pn(x,y)
%



% Compute Bc from cosmag
function bc = getbc(P, cosmag, isky, x, y)

  NA = 4;
  P1 = P( 1:NA );
  P2 = P( NA + 1 : NA + 3);
  P3 = P( NA + 4 : NA + 8);

  bc = P1(1)+P1(2)*exp(-P1(3)*exp(cosmag*P1(4))) + ...
       P2(1)*isky + P2(2)*isky.^2 + P2(3)*isky.^2 + ...
       P3(1)*x + P3(2)*y + P3(3)*x.*y + P3(4)*x.^2 + P3(5)*y.^2;

% P1(1)+P1(2)./(exp(P1(3)*(cosmag-P1(5)))+exp(P1(4)*(cosmag-P1(5)))+
% 1) + ...


end


% Regression function for leasqr()
function bc = lsfn( MM, P )
  cosmag = MM(:,1);
  isky   = MM(:,2);
  x      = MM(:,3);
  y      = MM(:,4);
  bc     = getbc(P, cosmag, isky, x, y);
end


function P = bc_approximation( Bc, cosmag, isky, x, y )

  p  = [ 21;-17;2500;0.325;  0;0;0; 0;0;0;0;0];
  MM = [ cosmag isky x y];

  [~, P,cvg,iter] = leasqr(MM, Bc, p, @lsfn, 1e-9, 1200); %  {stol,niter,wt,dp,dFdp,options}
  printf('cvg:%d iter:%d\n', cvg, iter);
  printf('P:\n');
  disp(P);

end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


arg_list = argv ();

FLIST = {};
outname = '';
K = 3;
NPASS = 5;
apply = 0;


i = 0;
while ( ++i <= nargin )

  if ( strcmp(arg_list{i},'--help') )
    show_usage(stdout);
    exit(0);

  elseif ( strcmp(arg_list{i},'-o') )

    if ( ++i > nargin )
      fprintf(stderr,'missing output name after -o switch\n');
      show_usage(stderr);
      exit(0);
    end

    outname = arg_list{i}

  elseif ( strcmp(arg_list{i},'-K') )

    if ( ++i > nargin )
      fprintf(stderr,'missing K after -K switch\n');
      show_usage(stderr);
      exit(0);
    end

    [K, ok] = str2num(arg_list{i})
    if ( !ok )
      fprintf(stderr,'Syntax error: %s\n',arg_list{i});
      show_usage( stderr );
      exit(1);
    end


  elseif ( strcmp(arg_list{i},'-N') )

    if ( ++i > nargin )
      fprintf(stderr,'missing NPASS after -N switch\n');
      show_usage(stdout);
      exit(0);
    end

    [NPASS, ok] = str2num(arg_list{i})
    if ( !ok )
      fprintf(stderr,'Syntax error: %s\n',arg_list{i});
      show_usage( stderr );
      exit(1);
    end

  elseif ( strcmp(arg_list{i},'-A') )
    apply = 1
  else
    FLIST{size(FLIST,2)+1} = arg_list{i};
  end

end


if ( size(FLIST,2) < 1 )
  show_usage( stderr );
  exit(1);
end




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



for i = 1:size(FLIST,2)

  fname = FLIST{i};
  printf('%s\n', fname);

  [~, ~, ~, m] = regexp(fname,'\d+');
  [plateid,ok] = str2num(m{1});
  if ( !ok )
    fprintf(stderr,'Can not get plateid from file name %s\n', fname);
    continue;
  end

  % check if this plate is already processed
  if ( ~strcmp(outname,'') && exist(outname,'file') )
     status = system(sprintf('grep -q "^%d" %s', plateid, outname ));
     if ( status == 0 )
       printf('SKIP %s: ALREADY PROCESSED\n', fname);
       continue;
     end
  end


  [cosmag, isky, x, y, el, Bc, C, ra, dec ] = loadmagrefs( fname );
  if ( size(cosmag) < 1 )
    continue;
  end

  if ( nnz(C==3) < 9 )
     fprintf(stderr, 'Not enough GSPC stars in %s\n', fname);
     continue;
  end


  for pass = 1:NPASS

    P = bc_approximation( Bc, cosmag, isky, x, y );
    Bcc = getbc(P, cosmag, isky, x, y);
    dBc = Bc - Bcc;
    sigma = std(dBc);

    fprintf(stderr,'* %d sigma=%g rows=%d\n',pass, sigma,rows(Bc));

    if ( K == 0 )
      break;
    end

    if ( pass < NPASS )

      if ( all( good=abs(dBc) < K * sigma) )
	break;
      end

      cosmag = cosmag(good);
      isky   = isky(good);
      x      = x(good);
      y      = y(good);
      el     = el(good);
      Bc     = Bc(good);
      C      = C(good);
      ra     = ra(good);
      dec    = dec(good);
    end

  end


 if ( ~strcmp(outname,'') )
    % save regression coefficients

    if ( ~exist(outname,'file') )
      fid = fopen(outname,'w');
      if ( fid == -1 )
        fprintf(stderr,'can not write %s\n', outname);
        exit (1);
      end

      fprintf(fid,'pid\tN\tS');
      for i = 1:rows(P)
        fprintf(fid,'\tP%d',i);
      end
      fprintf(fid,'\n');
      fclose(fid);
    end

    fid = fopen(outname,'a');
    if ( fid == -1 )
      fprintf(stderr,'can not write %s\n', outname);
      exit (1);
    end

    fprintf( fid, '%d\t%d\t%.3f', plateid, rows(Bc), sigma );
    for i = 1:rows(P)
     fprintf(fid,'\t%+.9E',P(i));
    end
    fprintf(fid,'\n');
    fclose(fid);
  end


  if (1)
    fname = sprintf('%s.out',fname);
    fid = fopen(fname,'w');
    fprintf(fid,'ra\tdec\tcosmag\tisky\tx\ty\tel\tBc\tC\tBcc\tdBc\n');
    fprintf(fid,'%.9f\t%+.9f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.0f\t%.3f\t%.3f\n',
         transpose([ra, dec, cosmag, isky, x, y, el, Bc, C, Bcc, dBc]));
    fclose(fid);
  end

  if ( apply )

    fprintf(stderr,'Loading targets...\n');

    [ objID ra dec xmin xmax ymin ymax area ipeak cosmag isky ...
      x y aU bU thetaU aI bI thetaI class pa ...
      ap1 ap2 ap3 ap4 ap5 ap6 ap7 ap8 ...
      blend quality prfStat prfMag gMag sMag ] = load_targets(plateid);

    fprintf(stderr,'Compute Bc...\n');

    Bc = getbc(P, cosmag, isky*1e-7 - 2.5, x-175000, y-175000);


    fprintf(stderr,'Saving data...\n');

    fname = sprintf('%d.bc.dat',plateid);
    fid = fopen(fname,'w');
    header = [ 'objID\tra\tdec\txmin\txmax\tymin\tymax\tarea\tipeak\tcosmag\t' ...
             'isky\tx\ty\taU\tbU\tthetaU\taI\tbI\tthetaI\tclass\t' ...
	     'pa\t\tap1\tap2\tap3\tap4\tap5\tap6\tap7\tap8\t' ...
	     'blend\tquality\tprfStat\tprfMag\tgMag\tsMag\tBc\n' ...
	   ];
	    
    fmt =    [ '%.0f\t%.9f\t%+.9f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t' ...
             '%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.0f\t' ...
	     '%.3f\t\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t' ...
	     '%.0f\t%.0f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n' ...
	   ];

    data =   [ objID ra dec xmin xmax ymin ymax area ipeak cosmag isky ...
             x y aU bU thetaU aI bI thetaI class pa ...
	     ap1 ap2 ap3 ap4 ap5 ap6 ap7 ap8 ...
	     blend quality prfStat prfMag gMag sMag Bc ];


    fprintf(fid,header);
    fprintf(fid,fmt, transpose(data));
    fclose(fid);

  end

  fprintf(stderr,'Finished\n');
  
end
