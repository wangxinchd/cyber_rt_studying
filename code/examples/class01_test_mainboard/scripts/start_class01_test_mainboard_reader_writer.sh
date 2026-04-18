

#!/bin/bash

TOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CYBER_RT_STUDYING_PATH=$(echo "$TOP_DIR" | sed 's|\(.*\)/install.*|\1|')

export GLOG_logtostderr=1
export GLOG_v=2
export GLOG_minloglevel=0
# cyber.pb.conf的配置文件会在这个路径中找。配置文件路径是 CYBER_PATH + conf/cyber.pb.conf

export CYBER_PATH=$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/

source ${CYBER_RT_STUDYING_PATH}/install/start_env.sh

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/lib/libclass01_test_mainboard_writer.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/dag/class01_test_mainboard_writer.dag"

sed -i "s|module_library : \".*\"|module_library : \"$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/lib/libclass01_test_mainboard_reader.so\"|" \
"$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/dag/class01_test_mainboard_reader.dag"

${CYBER_RT_STUDYING_PATH}/install/bin/mainboard -d "$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/dag/class01_test_mainboard_writer.dag" -d "$CYBER_RT_STUDYING_PATH/install/examples/class01_test_mainboard/dag/class01_test_mainboard_reader.dag"