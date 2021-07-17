#/bin/bash
# ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB.txt 2>&1
ulimit -c unlimited
ulimit -n 65535

today=`date +%Y-%m-%d-%H:%M:%S`

#load
mod="raid0-load-375M"
echo "load start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/raid0/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > load1KB20GB_${today}_${mod}.txt 2>&1

sudo cp -r /mnt/raid0/testRocksdb /mnt/raid0/testRocksdbBackup

#workloada
mod="raid0-workloada-375M"
echo "runA start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/raid0/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB_${today}_${mod}.txt 2>&1


sudo rm -rf /mnt/raid0/testRocksdb
sudo cp -r /mnt/raid0/testRocksdbBackup /mnt/raid0/testRocksdb
#workloadb
mod="raid0-workloadb-375M"
echo "runB start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/raid0/testRocksdb -threads 40 -P ../workloads/workloadb1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB_${today}_${mod}.txt 2>&1


sudo rm -rf /mnt/raid0/testRocksdb
sudo cp -r /mnt/raid0/testRocksdbBackup /mnt/raid0/testRocksdb
#workloadb
mod="raid0-workloadd-375M"
echo "runD start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/raid0/testRocksdb -threads 40 -P ../workloads/workloadd1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB_${today}_${mod}.txt 2>&1

