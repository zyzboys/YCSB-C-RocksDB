#/bin/bash
# ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB.txt 2>&1
ulimit -c unlimited
ulimit -n 65535

today=`date +%Y-%m-%d-%H:%M:%S`
ssd="/mnt/sdb"


# #load
# mod="multi-load-375M"
# echo "load start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloada1KB10GB.spec -dbConfig ../db_config/rocksdb_config.ini > load1KB10GB_${today}_${mod}.txt 2>&1

# sudo cp -r ${ssd}/testRocksdb ${ssd}/testRocksdbBackup

# #workloada
# mod="multi-workloada-375M"
# echo "runA start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloada1KB10GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB10GB_${today}_${mod}.txt 2>&1


# sudo rm -rf ${ssd}/testRocksdb
# sudo cp -r ${ssd}/testRocksdbBackup/* ${ssd}/testRocksdb
# #workloadb
# mod="multi-workloadb-375M"
# echo "runB start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloadb1KB10GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB10GB_${today}_${mod}.txt 2>&1


# sudo rm -rf ${ssd}/testRocksdb
# sudo cp -r ${ssd}/testRocksdbBackup/* ${ssd}/testRocksdb
# #workloadb
# mod="multi-workloadd-375M"
# echo "runD start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloadd1KB10GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB10GB_${today}_${mod}.txt 2>&1




#load
mod="multi-load-375M"
echo "load start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > load1KB20GB_${today}_${mod}.txt 2>&1

sudo cp -r ${ssd}/testRocksdb ${ssd}/testRocksdbBackup

#workloada
mod="multi-workloada-375M"
echo "runA start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB_${today}_${mod}.txt 2>&1


sudo rm -rf ${ssd}/testRocksdb
sudo cp -r ${ssd}/testRocksdbBackup/* ${ssd}/testRocksdb
#workloadb
mod="multi-workloadb-375M"
echo "runB start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloadb1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB_${today}_${mod}.txt 2>&1


sudo rm -rf ${ssd}/testRocksdb
sudo cp -r ${ssd}/testRocksdbBackup/* ${ssd}/testRocksdb
#workloadd
mod="multi-workloadd-375M"
echo "runD start"
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloadd1KB20GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB20GB_${today}_${mod}.txt 2>&1
