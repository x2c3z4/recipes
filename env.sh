#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH=$PATH:$DIR/thirdparty/blade:$DIR/thirdparty/perftools/bin
export PPROF_PATH=$DIR/thirdparty/perftools/bin/pprof

# export GLOG_alsologtostderr=1
export GLOG_logtostderr=1
