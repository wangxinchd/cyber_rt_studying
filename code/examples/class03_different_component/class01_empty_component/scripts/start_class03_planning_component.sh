#!/bin/bash

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')

export GLOG_logtostderr=1
export GLOG_v=2
export GLOG_minloglevel=0

source ${CYBER_RT_STUDYING_PATH}/install/start_env.sh

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class01_empty_component/lib/libtest_class03_planning.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class01_empty_component/dag/class03_planning.dag"

export CYBER_PATH=$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class01_empty_component/

${CYBER_RT_STUDYING_PATH}/install/bin/mainboard -d "$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class01_empty_component/dag/class03_planning.dag"
