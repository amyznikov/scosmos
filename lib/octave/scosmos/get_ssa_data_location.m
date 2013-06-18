% 
% datadir = get_ssa_data_location( surveyid )
%   Return path to ssa plates directory
%
%   Arguments:
%       surveyid: SSA survey index (0-9)
%
function datadir = get_ssa_data_location( surveyid )

  hostname = sscanf(syscall('hostname'),'%s');
  
  switch ( surveyid )
    case 1
      surveyname='SERC-J';
    case 2
      surveyname='SERC-R';
    case 3
      surveyname='SERC-I';
    case 4
      surveyname='ESO-R';
    otherwise 
      surveyname = sprintf('%d',surveyid);
  end
  
  
  if ( strcmp(hostname,'jup') ) % my home machine for debugging
    datadir = sprintf('/wsdb/catalogs/scosmos/%s', surveyname);
  elseif ( strcmp(hostname,'centos1') ) % KhAO linux machine
    datadir = sprintf('/mnt/catalogs/scosmos/%s/plates', surveyname);
  else
    error('Path to scosmos data root directory is not set');
  end

end

