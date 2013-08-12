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


  % check if this plate is already processed
  if ( ~strcmp(outname,'') && exist(outname,'file') )
     status = system(sprintf('fgrep -q "%s" %s', fname, outname ));
     if ( status == 0 )
       printf('SKIP %s: ALREADY PROCESSES\n', fname);
       continue;
     end
  end



  % load Bj,Vj,scosmos
 
  printf('\n');
  printf('%s\n',fname);

  columns = 'Johnson_B,Johnson_V,cosmag,isky,ra,dec,x,y';

  cond = [ '@Johnson_B!=\"NA\" && @Johnson_V!=\"NA\" && ' ...
           '(@Johnson_B-@Johnson_V)>-0.5 && (@Johnson_B-@Johnson_V)<2.5 && ' ...
           '@Johnson_B>10 && @Johnson_V>9.5 &&' ...
           '@cosmag>-26 && @cosmag<-23 && '  ...
	   'abs(@prfStat) < 4'
          ];

  cmd = sprintf('ccut %s -f "%s" -x "%s"', fname, columns, cond);

  pid = popen(cmd,'r');
  if ( pid == -1 ||  ~ischar(fgets(pid)) )
    fprintf(stderr,'can not read %s\n', fname);
  else
    v = transpose( fscanf(pid,'%lf',[8,Inf]) );
  end
  pclose(pid);

  if ( size(v,1) < 3 || size(v,2) != 8 )
    fprintf(stderr,'empty dataset %s\n', fname);
    continue;
  end


  % extract columns
  Bj     = v(:,1);
  Vj     = v(:,2);
  cosmag = v(:,3);
  isky   = v(:,4);
  ra     = v(:,5);
  dec    = v(:,6);
  x      = v(:,7); 
  y      = v(:,8); 
  RA     = mean(ra)*180/pi;
  DE     = mean(dec)*180/pi;

  % Make linear regression
  %  Bj = c0 + c1 * scosmos + c2*(Bj-Vj)
  %
  % The Bc will defined as:
  %  Bc = c0 + c1 * scosmos

  for pass = 1:NPASS

    MM = [ ones(size(cosmag)) cosmag (Bj-Vj) ];

    [A, sigma] = LinearRegression(MM, Bj);
    sigma = sqrt(sigma);

    printf('* %d sigma=%g rows=%d\n', pass, sigma, rows(MM));
    printf('A1 = %+g\n',A(1));
    printf('A2 = %+g\n',A(2));
    printf('A3 = %+g\n',A(3));
   

    if ( K == 0 || pass == NPASS || all(good=abs(Bj-MM*A)<K*sigma) )
      break;
    end

    Bj = Bj(good);
    Vj = Vj(good);
    cosmag = cosmag(good);
    isky = isky(good);
    ra = ra(good);
    dec = dec(good);
    x = x(good);
    y = y(good);

  end

  if ( ~strcmp(outname,'') )

    % save regression coefficients
    if ( ~exist(outname,'file') )
      fid = fopen(outname,'w');
      if ( fid == -1 )
        fprintf(stderr,'can not write %s\n', outname);
        exit (1);
      end
      fprintf(fid,'file\tRA\tDEC\tN\tS\tA0\tA1\tA2\n');
      fclose(fid);
    end


    fid = fopen(outname,'a');
    if ( fid == -1 )
      fprintf(stderr,'can not write %s\n', outname);
      exit (1);
    end

    fprintf( fid, '%s\t%7.3f\t%+8.3f\t%d\t%.3f\t%+.6f\t%+.6f\t%+.6f\n',
      fname, RA, DE, rows(cosmag), sigma, A(1), A(2), A(3) );
   fclose(fid);
  end

  fname = sprintf('%s.res',fname);
  fid = fopen(fname,'w');
  fprintf(fid,'ra\tdec\tx\ty\tr\tBj\tVj\tcosmag\tisky\tBc\tdM\n');
  
  Bc = A(1) + A(2) * cosmag;
  dM = Bj - A(3)*(Bj-Vj) - Bc;
  r  = sqrt((x - 177500).^2 + (y - 177500).^2); 

  fprintf(fid,'%.9f\t%+.9f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%+.3f\n',
   transpose([ra*180/pi dec*180/pi x y r Bj Vj cosmag isky Bc dM]));

  fclose (fid);

end

