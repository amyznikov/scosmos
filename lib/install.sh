#! /bin/bash

#
# OCTAVE_PATH=:/usr/local/lib/scosmos/octave
#
#

prefix=/usr/local
libdir=$prefix/lib/scosmos

mkdir -p $libdir/{octave,postgres} || exit 1
cp octave/* $libdir/octave/        || exit 1
cp postgres/* $libdir/postgres/    || exit 1

echo "To finish installation ensure that the path to $libdir/octave is added to your enviroment variable OCTAVE_PATH"
echo "For example export OCTAVE_PATH=:$libdir/octave"
