#/bin/bash
# ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB.txt 2>&1
ulimit -c unlimited
ulimit -n 65535

echo "load start"
../build/ycsbc -db leveldb load -dbPath /home/colin/hub/YCSB-C-RocksDB/build/testDir/vol1 -threads 1 -P ../workloads/levelDBworkloada1KB60GB.spec -dbConfig ../db_config/leveldb_config.ini > load1KB60GB_thread1_leveldb.txt 2>&1

# echo "run start"
# ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB80GB_1.txt 2>&1