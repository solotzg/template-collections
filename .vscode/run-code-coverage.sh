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
    -DUSE_CCACHE=OFF \
    -DTEST_LLVM_COVERAGE=ON \
    -GNinja

BIN=${DEBUG_BUILD_PATH}/${NAME}
cmake --build . --parallel ${NPROC}

LCOV_PATH=$(which lcov)
PROFILE_DIR=${DEBUG_BUILD_PATH}/.profile
REPORT_DIR=${DEBUG_BUILD_PATH}/.report

rm -rf ${PROFILE_DIR}
rm -rf ${REPORT_DIR}

export LLVM_PROFILE_FILE=${PROFILE_DIR}/code-cov-%12m.profraw

${BIN}

llvm-profdata merge -output=${PROFILE_DIR}/merged.profdata ${PROFILE_DIR}/code-cov-*.profraw
cd ${PROFILE_DIR}
llvm-cov \
    export \
    ${BIN} \
    --format=lcov \
    --instr-profile ${PROFILE_DIR}/merged.profdata \
    --ignore-filename-regex '/usr/include/.*' \
    --ignore-filename-regex '/usr/local/.*' \
    --ignore-filename-regex '/usr/lib/.*' \
    --ignore-filename-regex './cmake-build*' \
    --ignore-filename-regex './contrib/*' \
    >${PROFILE_DIR}/lcov.info

mkdir -p ${REPORT_DIR}
genhtml ${PROFILE_DIR}/lcov.info -o ${REPORT_DIR} --ignore-errors source
cd ${REPORT_DIR}
python3 -m http.server 8888
