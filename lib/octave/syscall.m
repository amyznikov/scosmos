%
% out = syscall( command, ... )
%   This is usual system() command but with forced generating error() if system() call fails
%
function out = syscall( cmd, varargin )
    cmd = sprintf('bash -o pipefail -c "%s"',sprintf(cmd, varargin{:}));
    # fprintf(stderr,'%s\n',cmd);
    [status, out] = system(cmd);
    if ( status ~= 0 )
        error ('subprocess (%s) fails with status %d.\nout is:\n''%s''',cmd, status,out);
    end
end
