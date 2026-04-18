#!/bin/bash

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')

export GLOG_logtostderr=1
export GLOG_v=2
export GLOG_minloglevel=0

source ${CYBER_RT_STUDYING_PATH}/install/start_env.sh

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/lib/libclass01_test_mainboard_reader.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/dag/class01_test_mainboard_reader.dag"


export CYBER_PATH=$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/

echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "ENV: $ENV"

${CYBER_RT_STUDYING_PATH}/install/bin/mainboard -d "$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/dag/class01_test_mainboard_reader.dag"