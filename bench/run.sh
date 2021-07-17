#/bin/bash
# ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB.txt 2>&1
ulimit -c unlimited
ulimit -n 65535

echo "load start"
./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > load1KB80GB_1.txt 2>&1

echo "run start"
./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB80GB_1.txt 2>&1