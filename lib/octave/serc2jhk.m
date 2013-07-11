%
% MM = serc2jhk( M1, M2, M3, S1, S2, S3 )
%   Accepted SERC to JHK conversion matrix
%
% args: 
%   M1:   Column of SERC-J prfMag
%   M2:   Column of SERC-R prfMag
%   M3    Column of SERC-I prfMag
%   S1:   Column of SERC-J isky
%   S2:   Column of SERC-R isky
%   S3:   Column of SERC-I isky
%

function MM = serc2jhk( M1, M2, M3, S1, S2, S3 )
  MM = [ ones(size(M1)) M1 M2 M3 S1 S2 S3 ];
end
