#! /usr/bin/octave -qf

%
% Перевод Bj,Vj в фильтр 'Bc' используя пересечение serc-j/apass
% в линейной части характеристической кривой:
%   Bc = a0 + a1*Bj + a2*(Bj-Vj) = b0 + b1 * scosmos ;
% 
% Система переопределена; произвол снимается избавившись
% от свободных нуль-пункта и множителя и минимизируя дисперсию за
% цвет:
%   Bj = c0 + c1 * scosmos + c2*(Bj-Vj)
%
% Решение системы выполняется по звездам apass.
%
%



pkg load optim;

arg_list = argv ();

FLIST = {};
outname = '';
K = 3;
NPASS = 5;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function show_usage( stream )
  fprintf(stream, 'USAGE:\n');
  fprintf(stream, '  %s [-o {output-name}] [-K K] [-N NPASS] apass.sercj.dat ...\n', program_name());
end


function MM = genmm( cosmag, isky, x, y, ex, c )
  MM = [ ones(size(cosmag)) cosmag cosmag.^2 cosmag.^3 ...
         x y x.*y x.^2 y.^2 ...
         isky isky.^2 isky.^3 ...
         ex ...
         c ... 
       ];
end


function MM = genmm2( cosmag, isky, x, y, ex )
  MM = [ ones(size(cosmag)) cosmag cosmag.^2 cosmag.^3 ...
         x y x.*y x.^2 y.^2 ...
         isky isky.^2 isky.^3 ...
         ex ...
       ];
end


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

  else
    FLIST{size(FLIST,2)+1} = arg_list{i};
  end

end


if ( size(FLIST,2) < 1 )
  show_usage(stderr);
  exit(1);
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


for i = 1:size(FLIST,2)

  v = [];
  fname = FLIST{i};


  [~, ~, ~, m] = regexp(fname,'\d+');
  [plateid,ok] = str2num(m{1});
  if ( !ok )
    fprintf(stderr,'can not get plateid from file name %s\n');
    continue;
  end

  % check if this plate is already processed
  if ( ~strcmp(outname,'') && exist(outname,'file') )
     status = system(sprintf('fgrep -q "^%d" %s', plateid, outname ));
     if ( status == 0 )
       printf('SKIP %s: ALREADY PROCESSES\n', fname);
       continue;
     end
  end



  % load Bj,Vj,scosmos
 
  printf('\n');
  printf('%s\n',fname);

  columns = 'Johnson_B,Johnson_V,cosmag,isky,ra,dec,objID,x,y,ex:(1.0-@bI/@aI)';

  cond = [ '@Johnson_B!=\"NA\" && @Johnson_V!=\"NA\" && ' ...
           '(@Johnson_B-@Johnson_V)>-0.75 && (@Johnson_B-@Johnson_V)<3.5 && ' ...
           '@Johnson_V>10 && @Johnson_B>10.5 &&' ...
	   'abs(@prfStat) < 5 && @class==2 && ' ...
	   '@aI>0 && @bI/@aI>0.5 && ' ...
           'abs(@B_err)<0.2 && abs(@Verr)<0.2 && ' ...
	   '@isky<6e7 && and(@quality,128)==0'
         ];


  cmd = sprintf('ccut %s -f "%s" -x "%s"', fname, columns, cond);

  pid = popen(cmd,'r');
  if ( pid == -1 ||  ~ischar(fgets(pid)) )
    fprintf(stderr,'can not read %s\n', fname);
  else
    v = transpose( fscanf(pid,'%lf',[10,Inf]) );
  end
  pclose(pid);

  if ( size(v,1) < 3 || size(v,2) != 10 )
    fprintf(stderr,'empty dataset %s\n', fname);
    continue;
  end


  % remove bad isky
  smin1 = min( v(:,4) ) + 1e6;
  smax1 = max( v(:,4) ) - 1e6;
  v = v(v(:,4) >= smin1 & v(:,4) <= smax1,:);


  % extract columns
  oBj    = v(:,1);
  oVj    = v(:,2);
  ocosmag= v(:,3);
  oisky  = v(:,4) * 1e-7;
  ora    = v(:,5);
  odec   = v(:,6);
  objID  = v(:,7);
  ox     = v(:,8)-177500;
  oy     = v(:,9)-177500;
  oex    = v(:,10);

  % select cosmag range 
  good   = ocosmag < -23.00 & ocosmag > -27.00;
  Bj     = oBj(good);
  Vj     = oVj(good);
  cosmag = ocosmag(good);
  isky   = oisky(good);
  x      = ox(good);
  y      = oy(good);
  ex     = oex(good);
  RA     = mean(ora) * 180/pi;
  DE     = mean(odec) * 180/pi;


  % Make regression
  %  Bj = P1(scosmos) + P2(isky) + P3(x,y) + C*(Bj-Vj)
  %
  % The Bc will defined as:
  %  Bc = P1(scosmos) + P2(isky) + P3(x,y)

  for pass = 1:NPASS

    MM = genmm(cosmag, isky, x, y, ex, (Bj-Vj));

    [A, sigma] = LinearRegression(MM, Bj);
    sigma = sqrt(sigma);

    printf('* %d sigma=%g rows=%d\n', pass, sigma, rows(MM));
    for i = 1:size(A,1)
      printf('A%d = %+g\n',i,A(i));
    end

    if ( K == 0 || pass == NPASS || all(good=abs(Bj-MM*A)<K*sigma) )
      break;
    end

    Bj = Bj(good);
    Vj = Vj(good);
    cosmag = cosmag(good);
    isky = isky(good);
    x = x(good);
    y = y(good);
    ex = ex(good);
  end

  if ( ~strcmp(outname,'') )

    % save regression coefficients
    if ( ~exist(outname,'file') )
      fid = fopen(outname,'w');
      if ( fid == -1 )
        fprintf(stderr,'can not write %s\n', outname);
        exit (1);
      end
      
      fprintf(fid,'pid\tRA\tDEC\tN\tS');
      for i = 1:size(A,1)
	fprintf(fid,'\tA%d',i);
      end
      fprintf(fid,'\n');
      fclose(fid);
    end


    fid = fopen(outname,'a');
    if ( fid == -1 )
      fprintf(stderr,'can not write %s\n', outname);
      exit (1);
    end

    fprintf( fid, '%d\t%7.3f\t%+8.3f\t%d\t%.3f', 
        plateid, RA, DE, rows(cosmag), sigma );
    for i = 1:size(A)
     fprintf(fid,'\t%+.6E',A(i));
    end
    fprintf(fid,'\n');
    fclose(fid);

  end

  fname = sprintf('%s.res',fname);
  fid = fopen(fname,'w');
  fprintf(fid,'objID\tex\tx\ty\tra\tdec\tBj\tVj\tcosmag\tisky\tBc\tdM\n');
  
  MM = genmm2(ocosmag, oisky, ox, oy, oex ); 
  AA = A([1:(size(A,1)-1)]);
  Bc = MM * AA;
  dM = oBj - A(size(A,1))*(oBj-oVj) - Bc;

  fprintf(fid,'%.0f\t%.3f\t%.3f\t%.3f\t%.9f\t%+.9f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%+.3f\n',
    transpose([objID oex ox oy ora*180/pi odec*180/pi oBj oVj ocosmag oisky Bc dM]));

  fclose (fid);

end

