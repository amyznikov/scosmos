%
% pid = popenq( fname, columns )
%  Uses awk to extract named columns from tsv file and create pipe to read it.
%  Returns pipe id, or -1 on error.
%  Use pclose() to close the pipe.
%
%  Example:
%   pid = popenq("somefile.dat","x,y,z");
%   if ( pid == -1 )
%     error("popenq() fails");
%   else
%    v = fscanf(pid,"%lf",[3,Inf]);
%    x = v(1,:);
%    y = v(2,:);
%    z = v(3,:);
%    ...
%   end
%
function pid = popenq( fname, columns )

  pid = fopen(fname,'r');

  if ( pid != -1 )

      headline = fgets(pid);
      fclose(pid);
      pid = -1;

      headers = strsplit(headline," \t\n\r", 1);
%      printf("** headers:\n");
%      disp(headers);
      
      cnames  = strsplit(columns," \t\r\n;,",1);
%			printf("** cnames:\n");
%			disp(cnames);

%      printf("\n");
            
      cmd = 'awk ''{ print ';

      for i = 1 : size(cnames,2)
        for j = 1 : size(headers,2)
          if ( strcmp( headers{j}, cnames{i} ) )
            break;
          end
        end

       if ( j > size(headers,2) )
          error ('no column ''%s'' found in ''%s''',cnames{i}, fname);
        end

        cmd = cstrcat(cmd, sprintf('$%d',j));
        if ( i < size(cnames,2) )
          cmd = cstrcat(cmd,'"\t"');
        end
      end

      cmd = cstrcat(cmd, sprintf('}'' \"%s\"', fname));

      pid = popen(cmd,'r');
      if ( pid != -1 )
          fgets(pid); % drop header
      end
  end

end
