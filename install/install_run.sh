#!/usr/bin/bash
INSTALL_RUN_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
echo "Cyber RT install path is: ";
echo $INSTALL_RUN_PATH;
export LD_LIBRARY_PATH=$INSTALL_RUN_PATH/lib:$LD_LIBRARY_PATH
