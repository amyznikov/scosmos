function Bj = T2Bj( field, BT, VT )
  A = getT2Bj(field);
  Bj = A(1) + A(2)*BT + A(3)*(BT-VT);
end
