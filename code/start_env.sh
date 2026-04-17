#! /usr/bin/bash

# 启动环境

# /mnt/sd_data/git_project/sim_plan/sim_plan/cyber_rt_studying/code/env/lib

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')

export LD_LIBRARY_PATH=${CYBER_RT_STUDYING_PATH}/code/env/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${CYBER_RT_STUDYING_PATH}/install/lib:$LD_LIBRARY_PATH

# ldd ${CYBER_RT_STUDYING_PATH}/install/bin/mainboard