#!/bin/bash

set -ue

source $(dirname $0)/utils.sh

${RELEASE_BUILD_PATH}/${NAME}
