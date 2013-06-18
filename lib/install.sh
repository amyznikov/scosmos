#! /bin/bash

#
# OCTAVE_PATH=:/usr/local/lib/scosmos/octave
# PATH=:/usr/local/lib/scosmos/scripts
#

prefix=/usr/local
libdir=$prefix/lib/scosmos

mkdir -p $libdir/{octave,postgres,scripts} || exit 1
cp octave/* $libdir/octave/                || exit 1
cp postgres/* $libdir/postgres/            || exit 1
cp scripts/* $libdir/scripts/              || exit 1

echo "Make sure that the path to $libdir/octave is added to your enviroment variable OCTAVE_PATH"
echo "and path to $libdir/scripts is added to your enviroment variable PATH"
echo "Use:"
echo "  export OCTAVE_PATH=:$libdir/octave"
echo "  export PATH=\$PATH:$libdir/scripts"
