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
    -DENABLE_THINLTO=OFF \
    -DENABLE_PCH=ON \
    -DUSE_CCACHE=OFF \
    -DARCHNATIVE=ON \
    -DTEST_LLVM_COVERAGE=ON \
    -GNinja

rm -rf ${WORKSPACE_FOLDER}/cmake-build-debug/CPPTest
cmake --build . --parallel ${NPROC}

LCOV_PATH=$(which lcov)
PROFILE_DIR=${WORKSPACE_FOLDER}/cmake-build-debug/.profile
REPORT_DIR=${WORKSPACE_FOLDER}/cmake-build-debug/.report

rm -rf ${PROFILE_DIR}
rm -rf ${REPORT_DIR}

export LLVM_PROFILE_FILE=${PROFILE_DIR}/code-cov-%12m.profraw
${WORKSPACE_FOLDER}/cmake-build-debug/CPPTest
llvm-profdata merge -output=${PROFILE_DIR}/merged.profdata ${PROFILE_DIR}/code-cov-*.profraw
cd ${PROFILE_DIR}
llvm-cov export ${WORKSPACE_FOLDER}/cmake-build-debug/CPPTest --format=lcov \
    --instr-profile ${PROFILE_DIR}/merged.profdata \
    --ignore-filename-regex '/usr/include/.*' \
    --ignore-filename-regex '/usr/local/.*' \
    --ignore-filename-regex '/usr/lib/.*' \
    --ignore-filename-regex './cmake-build*' \
    >${PROFILE_DIR}/lcov.info

mkdir -p ${REPORT_DIR}
genhtml ${PROFILE_DIR}/lcov.info -o ${REPORT_DIR} --ignore-errors source
