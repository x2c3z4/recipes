#!/bin/bash
set -o nounset          # Treat unset variables as an error

LINUX=$1
ANDROID=$2
OUTDIR=diff-out

SUBDIR=`ls $ANDROID`
#if ! [ -e $(pwd)/${OUTDIR} ];then
#  mkdir $(pwd)/${OUTDIR} 
#fi
  
rm -rf $(pwd)/${OUTDIR} 
mkdir $(pwd)/${OUTDIR} 

diff -ur ${LINUX} ${ANDROID}  >${OUTDIR}/a.diff 

for basename in $SUBDIR
do
  path1="${LINUX}/${basename}"
  path2="${ANDROID}/${basename}"
  if [ -d ${path1} -a -d ${path2} ];then
    diff -qr ${path1} ${path2} | sed '/^Common/d' >${OUTDIR}/${basename}.diff 
    #diff -qr ${path1} ${path2} >${OUTDIR}/${basename}.diff 
    diff -ur ${path1} ${path2} >${OUTDIR}/${basename}"-detail".diff 
  fi
done

