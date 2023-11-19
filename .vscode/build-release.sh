#!/usr/bin/env bash

set -ue

source $(dirname $0)/utils.sh
NPROC=$(get_core_num)

mkdir -p ${RELEASE_BUILD_PATH} && cd ${RELEASE_BUILD_PATH}

CC=${CC} CXX=${CXX} cmake -S ${WORKSPACE_FOLDER} \
    -DCMAKE_BUILD_TYPE=RELWITHDEBINFO \
    -Wno-dev \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DENABLE_TIME_TRACES=OFF \
    -DENABLE_THINLTO=${ENABLE_THINLTO} \
    -DENABLE_PCH=${ENABLE_PCH} \
    -DUSE_CCACHE=${USE_CCACHE} \
    -DARCHNATIVE=${ARCHNATIVE} \
    ${CMAKE_BUILD_RPATH} \
    -GNinja \
    ${EXTRA_CMAKE_ARGS}

cmake --build . --parallel ${NPROC}
