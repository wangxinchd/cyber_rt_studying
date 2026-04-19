#!/bin/bash

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')

export GLOG_logtostderr=1
export GLOG_v=2
export GLOG_minloglevel=0

source ${CYBER_RT_STUDYING_PATH}/install/start_env.sh

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class02_component_one_input/lib/libtest_class02_loc.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class02_component_one_input/dag/class02_loc.dag"

export CYBER_PATH=$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class02_component_one_input/

${CYBER_RT_STUDYING_PATH}/install/bin/mainboard -d "$CYBER_RT_STUDYING_PATH/install/examples/class03_different_component/class02_component_one_input/dag/class02_loc.dag"
