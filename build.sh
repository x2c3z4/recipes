#!/bin/bash
source env.sh

if [[ "$1" = "test" ]];then
  blade test -pdebug ...
else
  blade build -pdebug ...
fi
