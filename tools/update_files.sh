#!/bin/bash

files=`find .`
if [ $# != 1 ];then
  echo "Usage: update-headers.sh src"
  exit
fi
for f in $files;do
  cmd="cp $1/$f $f"
  # echo $cmd
  `$cmd`
done
