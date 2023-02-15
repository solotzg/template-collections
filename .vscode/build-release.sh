#!/bin/bash

set -ue

source $(dirname $0)/utils.sh
NPROC=$(get_core_num)

mkdir -p ${RELEASE_BUILD_PATH} && cd ${RELEASE_BUILD_PATH}

CC=${CC} CXX=${CXX} cmake -S ${WORKSPACE_FOLDER} \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -Wno-dev \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DENABLE_TIME_TRACES=0 \
    -DENABLE_THINLTO=${ENABLE_THINLTO} \
    -DENABLE_PCH=${ENABLE_PCH} \
    -DUSE_CCACHE=OFF \
    -DARCHNATIVE=ON \
    -DENABLE_BENCHMARK=${ENABLE_BENCHMARK}\
    -GNinja

cmake --build . --parallel ${NPROC}
