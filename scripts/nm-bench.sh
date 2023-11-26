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

nm -C ${SRCPATH}/cmake-build-release/bench/bench
