# YCSB-C

Yahoo! Cloud Serving Benchmark in C++, a C++ version of YCSB (https://github.com/brianfrankcooper/YCSB/wiki)

## Quick Start

To build YCSB-C on Ubuntu, for example:

```
$ sudo apt-get install libtbb-dev
$ mkdir -p build && cd build
$ cmake ..
$ make
```

Run Workload A with a [RocksDB](https://github.com/facebook/rocksdb)-based
implementation of the database, for example:
```
./ycsbc -db rocksdb load -threads 4 -dbPath ./test -P workloads/workloada.spec -dbConfig ./RocksDBConfig/rocksdb_config.ini
```
Also reference run.sh and run.sh for the command line. See help by
invoking `./ycsbc` without any arguments.

Note that we do not have load and run commands as the original YCSB. Specify
how many records to load by the recordcount property. Reference properties
files in the workloads dir.

