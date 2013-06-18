%
% out = pgquery( sqlCommand, ... )
%   Executes sql query on wsdb database.
%
%   Example:
%   plate_list = sscanf(pgquery( "select plateid from ssa_plates where surveyid=1 and decpnt=-30" ),"%d");
%
function out = pgquery( sqlCommand, varargin )
  cmd = sprintf('bash -o pipefail -c "psql wsdb -c \\"copy (%s) to stdout\\""',sprintf(sqlCommand, varargin{:}));
  [status, out] =  system(cmd);
  if ( status ~= 0 )
    error ("subprocess (%s) fails with status %d",cmd, status);
  end
end

