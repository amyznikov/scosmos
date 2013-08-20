%
% Bc = Johnson2Bc( Johnson_B, Johnson_V, plateid)
%
%   Johnson BV to scosmos Bc transformation for given plate
%  Due to interstellar reddering, the transformation is not contant 
%  and depends on region of the sky (mainly galactic latitude)
%
%
function Bc = Johnson2Bc( Johnson_B, Johnson_V, plateid)
  Bc = Johnson_B + getJohnson2Bc(plateid) * (Johnson_B - Johnson_V);
endfunction
