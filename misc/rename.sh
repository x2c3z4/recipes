#!/bin/bash
set -o nounset          # Treat unset variables as an error

for file in `ls`  
do  
  echo $file;
  newfile =`sed 's/chap/sect/' $file`  
  mv $file $newfile  
done 

