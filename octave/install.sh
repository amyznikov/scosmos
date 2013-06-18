#! /bin/bash

#
# OCTAVE_PATH=:/usr/local/lib/scosmos/
#

prefix=/usr/local
libdir=$prefix/lib/scosmos

mkdir -p $libdir || exit 1
 
cp scosmos/* $libdir/ || exit 1

echo "To finish installation ensure that the path to $libdir is added to your enviroment variable OCTAVE_PATH"
echo "For example export OCTAVE_PATH=:$libdir"

    

