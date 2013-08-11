function Vj = T2Vj( field, BT, VT )
  A = getT2Vj(field);
  Vj = A(1) + A(2)*VT + A(3)*(BT-VT);
end
