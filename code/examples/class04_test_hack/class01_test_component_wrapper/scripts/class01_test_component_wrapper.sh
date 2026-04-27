#!/bin/bash

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')

export GLOG_logtostderr=1
export GLOG_v=2
export GLOG_minloglevel=0

source ${CYBER_RT_STUDYING_PATH}/install/start_env.sh


# Update planning dag
sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class04_test_hack/class01_test_component_wrapper/lib/libclass04_class01_test_component_wrapper_planning.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class04_test_hack/class01_test_component_wrapper/dag/class01_test_component_wrapper.dag"

export CYBER_PATH=$CYBER_RT_STUDYING_PATH/install/examples/class04_test_hack/class01_test_component_wrapper/

${CYBER_RT_STUDYING_PATH}/install/bin/mainboard \
    -d "$CYBER_RT_STUDYING_PATH/install/examples/class04_test_hack/class01_test_component_wrapper/dag/class01_test_component_wrapper.dag"
