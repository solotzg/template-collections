#!/bin/bash

set -uex

SCRIPTPATH="$(
    cd "$(dirname "$0")"
    pwd -P
)"
SRCPATH=$(
    cd ${SCRIPTPATH}/..
    pwd -P
)
WORKSPACE_FOLDER=${SRCPATH}
${SCRIPTPATH}/run-code-coverage.sh
cp -r ${WORKSPACE_FOLDER}/cmake-build-debug/.report ~/windows/Downloads
