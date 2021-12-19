#!/bin/bash
target="rocksdb/leveldb"
if [ $# != 1 ]
then
    echo "need target($target) to build"
    exit
fi
PROJ_PATH=$(pwd)
CORE_NUM=$((`nproc`-1))
echo "cpu cores: $CORE_NUM"
if [ "$1" == "rocksdb" ]
then
    cd ${PROJ_PATH}/rocksdb && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j ${CORE_NUM} rocksdb
elif [ "$1" == "leveldb" ]
then
    cd ${PROJ_PATH}/leveldb && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j ${CORE_NUM} leveldb
else
    echo "target must be rocksdb or leveldb"
fi