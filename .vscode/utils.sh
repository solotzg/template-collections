SCRIPTPATH="$(
    cd "$(dirname "$0")"
    pwd -P
)"
SRCPATH=$(
    cd ${SCRIPTPATH}/..
    pwd -P
)
if [ -f ${SCRIPTPATH}/_env.sh ]; then
    source ${SCRIPTPATH}/_env.sh
fi

function get_core_num() {
    if [[ $(uname -s) == Darwin ]]; then
        NPROC=${NPROC:-$(sysctl -n hw.physicalcpu)}
    else
        NPROC=${NPROC:-$(nproc || grep -c ^processor /proc/cpuinfo)}
    fi
    echo ${NPROC}
}

export -f get_core_num

NAME=CPPTest
WORKSPACE_FOLDER=${SRCPATH}
DEBUG_BUILD_PATH=${WORKSPACE_FOLDER}/cmake-build-debug
RELEASE_BUILD_PATH=${WORKSPACE_FOLDER}/cmake-build-release
CC=${CC:-clang}
CXX=${CXX:-clang++}
ENABLE_PCH=${ENABLE_PCH:-ON}
ENABLE_THINLTO=${ENABLE_THINLTO:-ON}
ENABLE_BENCHMARK=${ENABLE_BENCHMARK:-OFF}
ARCHNATIVE=${ARCHNATIVE:-ON}
