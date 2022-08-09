#!/bin/bash
APP_RUN_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
CYBER_PATH=$APP_RUN_PATH
echo "Cyber RT run env path is: ";
echo $CYBER_PATH;
export CYBER_PATH

# method 1 ok
#source $APP_RUN_PATH/../../../env/env_run.sh
#source $APP_RUN_PATH/../../../../install/install_run.sh
#export LD_LIBRARY_PATH=.:./lib:$LD_LIBRARY_PATH

# method 2 ok
export LD_LIBRARY_PATH=$APP_RUN_PATH:./lib:$APP_RUN_PATH/../../../env/lib:$APP_RUN_PATH/../../../../install/lib:$LD_LIBRARY_PATH

argv=$1
./CyberRTReadWriteTest $argv
