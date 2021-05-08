#!/bin/bash
PROJ_PATH=$(pwd)
CORE_NUM=$((`nproc`-1))
echo "cpu cores: $CORE_NUM"
cd ${PROJ_PATH}/rocksdb && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j ${CORE_NUM}