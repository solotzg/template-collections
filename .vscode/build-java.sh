#!/bin/bash

set -ue

source $(dirname $0)/utils.sh

if [[ -z ${JAVA_HOME} ]]; then
    BIN="javac"
else
    BIN="${JAVA_HOME}/bin/javac"
fi

${BIN} ${1}
