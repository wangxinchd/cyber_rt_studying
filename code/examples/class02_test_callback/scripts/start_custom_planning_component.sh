#!/bin/bash

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')

export GLOG_logtostderr=1
export GLOG_v=2
export GLOG_minloglevel=0

export CYBER_PATH=$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/

source ${CYBER_RT_STUDYING_PATH}/install/start_env.sh

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/lib/libtest_planning.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/dag/class02_planning.dag"

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/lib/libtest_loc.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/dag/class02_loc.dag"

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/lib/libtest_custom_planning.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/dag/class02_custom_planning.dag"

${CYBER_RT_STUDYING_PATH}/install/bin/mainboard \
    -d "$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/dag/class02_planning.dag" \
    -d "$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/dag/class02_loc.dag" \
    -d "$CYBER_RT_STUDYING_PATH/install/examples/class02_test_callback/dag/class02_custom_planning.dag" 