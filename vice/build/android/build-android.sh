#!/bin/sh

VICEVERSION=2.4.1

# see if we are in the top of the tree
if [ ! -f configure.in ]; then
  cd ../..
  if [ ! -f configure.in ]; then
    echo "please run this script from the base of the VICE directory"
    exit 1
  fi
fi

curdir=`pwd`

# set all cpu builds to no
armbuild=no
arm7abuild=no
mipsbuild=no
x86build=no

showusage=no

# check options
for i in $*
do
  if test x"$i" = "xarmeabi"; then
    armbuild=yes
  fi
  if test x"$i" = "xarmeabi-v7a"; then
    arm7abuild=yes
  fi
  if test x"$i" = "xmips"; then
    mipsbuild=yes
  fi
  if test x"$i" = "xx86"; then
    x86build=yes
  fi
  if test x"$i" = "xhelp"; then
    showusage=yes
  fi
done

if test x"$showusage" = "xyes"; then
  echo "Usage: $0 [<cpu types>] [help]"
  echo "cpu-types: armeabi, armeabi-v7a, mips, x86."
  echo ""
  echo "if no cpu-type is given armeabi will be built by default."
  exit 1
fi

CPUS=""

if test x"$armbuild" = "xyes"; then
  CPUS="armeabi"
fi

if test x"$arm7abuild" = "xyes"; then
  if test x"$CPUS" = "x"; then
    CPUS="armeabi-v7a"
  else
    CPUS="$CPUS armeabi-v7a"
  fi
fi

if test x"$mipsbuild" = "xyes"; then
  if test x"$CPUS" = "x"; then
    CPUS="mips"
  else
    CPUS="$CPUS mips"
  fi
fi

if test x"$x86build" = "xyes"; then
  if test x"$CPUS" = "x"; then
    CPUS="x86"
  else
    CPUS="$CPUS x86"
  fi
fi

if test x"$CPUS" = "x"; then
  CPUS="armeabi"
fi

if test x"$CPUS" = "xarmeabi armeabi-v7a mips x86"; then
  CPULABEL="all"
else
  CPULABEL=$CPUS"
fi

cd src
echo generating src/translate_table.h
. ./gentranslatetable.sh <translate.txt >translate_table.h
echo generating src/translate.h
. ./gentranslate_h.sh <translate.txt >translate.h
echo generating src/infocontrib.h
. ./geninfocontrib_h.sh <../doc/vice.texi | sed -f infocontrib.sed >infocontrib.h
cd arch/android/AnVICE/jni
echo generating Application.mk
cp Application.proto Application.mk
echo >>Application.proto "APP_ABI := $CPUS"
cd ..
echo building libvice.so
ndk-build
echo generating apk
ant debug
cd ../../../..
mv src/arch/android/AnVICE/bin/PreConfig-debug.apk ./AnVICE-\($CPULABEL\)-$VICEVERSION.apk
