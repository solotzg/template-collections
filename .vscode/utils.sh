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

NAME=${NAME:-CppTest}
WORKSPACE_SRC_DIR=${WORKSPACE_SRC_DIR:-${SRCPATH}}
DEBUG_BUILD_PATH=${DEBUG_BUILD_PATH:-${WORKSPACE_SRC_DIR}/cmake-build-debug}
RELEASE_BUILD_PATH=${RELEASE_BUILD_PATH:-${WORKSPACE_SRC_DIR}/cmake-build-release}
CC=${CC:-clang}
CXX=${CXX:-clang++}
ENABLE_PCH=${ENABLE_PCH:-ON}
ENABLE_THINLTO=${ENABLE_THINLTO:-ON}
ENABLE_BENCHMARK=${ENABLE_BENCHMARK:-OFF}
ARCHNATIVE=${ARCHNATIVE:-ON}
BUILD_RPATH=${BUILD_RPATH:-}
USE_CCACHE=${USE_CCACHE:-ON}
EXTRA_CMAKE_ARGS=${EXTRA_CMAKE_ARGS:-}
JAVA_HOME=${JAVA_HOME:-}

CMAKE_BUILD_RPATH=""
if [[ -n ${BUILD_RPATH} ]]; then
    CMAKE_BUILD_RPATH="-DCMAKE_BUILD_RPATH=${BUILD_RPATH}"
fi
