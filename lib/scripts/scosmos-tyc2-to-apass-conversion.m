#! /usr/bin/octave -qf


pkg load optim;

arg_list = argv ();

FLIST = {};
outname = '';
color=''

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function show_usage( stream )
  fprintf(stream, 'USAGE:\n');
  fprintf(stream, '  %s [-o {output-name}] -c {V|B} apass.tyc2.dat ...\n', program_name());
end


i = 0;
while ( ++i <= nargin )

  if ( strcmp(arg_list{i},'--help') )
    show_usage(stdout);
    exit(0);

  elseif ( strcmp(arg_list{i},'-o') )

    if ( ++i > nargin )
      fprintf(stderr,'missing output name after -o argument\n');
      show_usage(stdout);
      exit(0);
    end
    outname = arg_list{i}

  elseif ( strcmp(arg_list{i},'-c') )

    if ( ++i > nargin )
      fprintf(stderr,'missing color name after -c argument\n');
      show_usage(stdout);
      exit(0);
    end
    color = arg_list{i}

  else
    FLIST{size(FLIST,2)+1} = arg_list{i};
  end

end


if ( size(FLIST,2) < 1 )
  show_usage(stderr);
  exit(1);
end

if ( strcmp(color,'' ) )
  fprintf(stderr,'Target color not specified\n');
  show_usage(stderr);
  exit(1);
end

if ( ~strcmp(color,'B') && ~strcmp(color,'V') )
  fprintf(stderr,'Invalid target color specified\n');
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



  % load BT,VT,Bj

  printf('\n');
  printf('%s\n',fname);


  switch color
    case 'B'  columns = 'btmag,vtmag,Johnson_B,radeg,decdeg';
    case 'V'  columns = 'btmag,vtmag,Johnson_V,radeg,decdeg';
  end

  cond = [ '@e_btmag<0.5 && @e_vtmag<0.5 && ' ...
           '(@btmag-@vtmag)>-0.5 && (@btmag-@vtmag)<3 && ' ...
           '@Johnson_B!=\"NA\" && @Johnson_V!=\"NA\" && ' ...
           '(@Johnson_B-@Johnson_V)>-0.5 && (@Johnson_B-@Johnson_V)<2.5 && ' ...
           '@Johnson_B>10 && @Johnson_V>9.5' ...
          ];

  cmd = sprintf('ccut %s -f "%s" -x "%s"', fname, columns, cond);

  pid = popen(cmd,'r');
  if ( pid == -1 ||  ~ischar(fgets(pid)) )
    fprintf(stderr,'can not read %s\n', fname);
  else
    v = transpose( fscanf(pid,'%lf',[5,Inf]) );
  end
  pclose(pid);

  if ( size(v,1) < 3 || size(v,2) != 5 )
    fprintf(stderr,'empty dataset %s\n', fname);
    continue;
  end


  % extract columns
  BT = v(:,1);
  VT = v(:,2);
  MJ = v(:,3);

  RA = mean(v(:,4));
  DE = mean(v(:,5));

  % make linear reduction Bj = A0 + A1*BT + A2*VT

  K = 3;
  NPASS = 5;

  for pass = 1:NPASS

    switch color
      case 'B'  MM = [ ones(size(MJ)) BT (BT-VT) ];
      case 'V'  MM = [ ones(size(MJ)) VT (BT-VT) ];
    end

    [A, sigma] = LinearRegression(MM, MJ);
    sigma = sqrt(sigma);

    printf('* %d sigma=%g rows=%d\n', pass, sigma, rows(MM));
    disp(A);

    if ( K == 0 || pass == NPASS || all(good=abs(MJ-MM*A)<K*sigma) )
      break;
    end

    BT = BT(good);
    VT = VT(good);
    MJ = MJ(good);

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
      fname, RA, DE, rows(MJ), sigma, A(1), A(2), A(3) );
   fclose(fid);
  end

end

