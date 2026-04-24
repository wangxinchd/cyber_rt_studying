#! /usr/bin/bash

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')


export LD_LIBRARY_PATH=${CYBER_RT_STUDYING_PATH}/code/env/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${CYBER_RT_STUDYING_PATH}/install/lib:$LD_LIBRARY_PATH

export CYBER_PATH=$TOP_DIR/..

${CYBER_RT_STUDYING_PATH}/install/bin/cyber_monitor