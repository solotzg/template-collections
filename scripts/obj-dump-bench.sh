#!/usr/bin/env bash

set -ue

SCRIPTPATH=$(
    cd "$(dirname "$0")"
    pwd -P
)
SRCPATH=$(
    cd ${SCRIPTPATH}/..
    pwd -P
)

objdump -C -r -d ${SRCPATH}/cmake-build-release/bench/bench
