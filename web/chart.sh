#!/bin/bash
set -o nounset          # Treat unset variables as an error
rm js/data.js
cd data

f=bugs.dat

head="new Morris.Line({ element: '"${f%%.*}"', data: ["
end="], xkey: 'date', ykeys: ['s1s2', 's3', 's4'], labels: ['s1-s2', 's3', 's4'], ymin: '0', xLabels: 'day', postUnits: '', });"
content=`awk '{printf("{ date: '\''%s'\'', s1s2: %s, s3: %s, s4: %s},\n", $1, $2, $3, $4)}'  $f`
cd ..

echo $head $content $end >> js/data.js
