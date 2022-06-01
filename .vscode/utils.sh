function get_core_num() {
    if [[ $(uname -s) == Darwin ]]; then
        NPROC=${NPROC:-$(sysctl -n hw.physicalcpu)}
    else
        NPROC=${NPROC:-$(nproc || grep -c ^processor /proc/cpuinfo)}
    fi
    echo ${NPROC}
}

export -f get_core_num
