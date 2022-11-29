#!/bin/bash

set -ue

SCRIPTPATH="$(
    cd "$(dirname "$0")"
    pwd -P
)"
SRCPATH=$(
    cd ${SCRIPTPATH}/..
    pwd -P
)
source ${SCRIPTPATH}/utils.sh
NPROC=$(get_core_num)

WORKSPACE_FOLDER=${SRCPATH}
mkdir -p ${WORKSPACE_FOLDER}/cmake-build-debug
cd ${WORKSPACE_FOLDER}/cmake-build-debug

CC=${CC:-clang}
CXX=${CXX:-clang++}

CC=${CC} CXX=${CXX} cmake -S ${WORKSPACE_FOLDER} \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -Wno-dev \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DENABLE_TIME_TRACES=0 \
    -DENABLE_THINLTO=0 \
    -DENABLE_PCH=ON \
    -DUSE_CCACHE=OFF \
    -DTEST_LLVM_COVERAGE=OFF \
    -GNinja
