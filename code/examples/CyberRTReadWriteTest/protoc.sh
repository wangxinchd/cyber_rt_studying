#!/usr/bin/bash

_SETUP_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
LD_LIBRARY_PATH=$_SETUP_PATH/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
