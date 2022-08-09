#!/usr/bin/bash
ENV_RUN_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
echo "Third party env path is: ";
echo $ENV_RUN_PATH;
export LD_LIBRARY_PATH=$ENV_RUN_PATH/lib:$LD_LIBRARY_PATH
