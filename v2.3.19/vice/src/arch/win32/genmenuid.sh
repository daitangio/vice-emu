#!/bin/sh
idm_counter=100

echo "/*"
echo " * menuid.h"
echo " *"
echo " * Autogenerated by genmenuid.sh, DO NOT EDIT !!!"
echo " *"
echo " */"
echo "#ifndef VICE_MENUID_H"
echo "#define VICE_MENUID_H"
echo "static struct { char *str; int cmd; } idmlist[] = {"

while read data
do
  ok="no"
  case ${data%%_*} in
    "IDM") echo "  { \"$data\", $idm_counter },"
           idm_counter=`expr $idm_counter + 1`
           ok="yes"
    ;;
  esac
done

echo " { NULL, 0 }"
echo "};"
echo "#endif"
