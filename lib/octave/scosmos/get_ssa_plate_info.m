%
% [A0, D0, EPOCH, X0, Y0] = get_ssa_plate_info( surveyid, plateid )
%   Return plate center and observation epoch for given plate.
% 
%   Args:
%       surveyid: SSA survey number (0..9)
%       plateid : SSA plate id as returned from get_ssa_plate_list()
%
%   Return:
%       A0     : nominal plate center RA, radians 
%       D0     : nominal plate center DEC, radians
%       EPOCH  : epoch of observation, years
%       X0     : Centre of scan area in X, microns
%       Y0     : Centre of scan area in Y, microns
%  
%   Example : 
%       [A0, D0, EPCH, X0, Y0] = get_ssa_plate_info( 1, 66109);
%

function [A0, D0, EPOCH, X0, Y0] = get_ssa_plate_info( surveyid, plateid )

  sqlCmd = sprintf('select rapnt, decpnt, epoch, xpnt, ypnt from ssa_plates where surveyid=%d and plateid=%d',
                    surveyid, plateid);

  A = sscanf( pgquery(sqlCmd) , '%lf' );
  A0 = A(1,1) * pi / 180;
  D0 = A(2,1) * pi / 180;
  EPOCH = A(3,1);
  X0 = A(4,1);
  Y0 = A(5,1);
end
