#! /bin/bash

#
# OCTAVE_PATH=:/usr/local/lib/scosmos/octave
# PATH=:/usr/local/lib/scosmos/scripts
#

prefix=/usr/local
libdir=$prefix/lib/scosmos
olss=0

function show_usage()
{
    echo "Usage:"
    echo "  ./install.sh [OPTIONS]"
    echo ""
    echo "With no arguments will install only scosmos octave functions into $libdir directory"
    echo ""
    echo "OPTIONS:"
    echo "  prefix=custom/install/prefix (default is $prefix)"
    echo "  libdir=custom/path/to/scosmos/octave/directoty (default is $libdir)"
    echo "  olss    will install octave-olss package into octave"
    echo ""
}


while [ -n "$1" ] ; do
    arg=$1;
    case $arg in
         prefix=*) prefix=${arg:7}  ;; 
         libdir=*) libdir=${arg:7}  ;;
         olss)     olss=1 ;;
         *) show_usage ; exit 1 ;;
    esac
    shift;
done


mkdir -p $libdir/{octave,postgres,scripts} || exit 1
cp octave/* $libdir/octave/                || exit 1
cp postgres/* $libdir/postgres/            || exit 1
cp scripts/* $libdir/scripts/              || exit 1

if (( ${olss} )) ; 
then 

echo ""
echo "Installing octave-olss..."
echo ""
make -C octave-olss/src clean && tar cfz octave-olss.tar.gz octave-olss/ || exit 1

{
cat << EOF
    pkg uninstall -verbose octave-olss
    pkg install -verbose octave-olss.tar.gz
EOF
} | octave -qf || exit 1

fi

   
echo ""
echo ""
echo "To finish installation make sure that the path to $libdir/octave "
echo "is added to your enviroment variable OCTAVE_PATH, "
echo "as well as the path to $libdir/scripts is added to your enviroment variable PATH"
echo "Use:"
echo "  export OCTAVE_PATH=:$libdir/octave"
echo "  export PATH=\$PATH:$libdir/scripts"
echo ""
