#!/bin/bash

set -ue

source $(dirname $0)/utils.sh

${DEBUG_BUILD_PATH}/${NAME}
