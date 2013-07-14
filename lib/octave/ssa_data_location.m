%
% path = ssa_data_location( surveyid_or_plateid )
%   Return path to ssa plates directory for given plate or survey id
%
%   Arguments:
%       surveyid_or_plateid:
%           One of SSA survey index (if in 1..9) or SSA plateid (if >= 65537)
%
function path = ssa_data_location( surveyid_or_plateid )

  hostname = sscanf(syscall('hostname'),'%s');

  if ( surveyid_or_plateid > 9 )
    surveyid_or_plateid = ssa_surveyid( surveyid_or_plateid );
  end

  switch ( surveyid_or_plateid )
    case 1
      surveyname='SERC-J';
    case 2
      surveyname='SERC-R';
    case 3
      surveyname='SERC-I';
    case 4
      surveyname='ESO-R';
    otherwise
      surveyname = sprintf('%d',surveyid_or_plateid);
  end


  if ( strcmp(hostname,'jup') ) % my home machine for debugging
    path = sprintf('/wsdb/catalogs/scosmos/%s', surveyname);
  elseif ( strcmp(hostname,'centos1') ) % KhAO linux machine
    path = sprintf('/mnt/catalogs/scosmos/%s/plates', surveyname);
  elseif ( strcmp(hostname,'hp') ) % my HP notebook
    path = sprintf('/opt/projects/scosmos/tests/mc/plates');
  else
    error('Path to scosmos data root directory is not set');
  end

end

