#!/usr/bin/env bash

set -ue

source $(dirname $0)/utils.sh
NPROC=$(get_core_num)

mkdir -p ${DEBUG_BUILD_PATH} && cd ${DEBUG_BUILD_PATH}

CC=${CC} CXX=${CXX} cmake -S ${WORKSPACE_FOLDER} \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -Wno-dev \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DENABLE_TIME_TRACES=OFF \
    -DENABLE_THINLTO=OFF \
    -DENABLE_PCH=${ENABLE_PCH} \
    -DUSE_CCACHE=${USE_CCACHE} \
    -DTEST_LLVM_COVERAGE=OFF \
    ${CMAKE_BUILD_RPATH} \
    -GNinja \
    ${EXTRA_CMAKE_ARGS}

cmake --build . --parallel ${NPROC}
