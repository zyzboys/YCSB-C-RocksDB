
ulimit -c unlimited
ulimit -n 65535

today=`date +%Y-%m-%d`

# 375MB/s
#sudo bash -c 'echo "9:127 375000000" > blkio.throttle.write_bps_device'

#sudo rm -rf /mnt/raid0/testRocksdb/*

# mod="single-noWAL-40T-375M-1"
# echo "load start"
# # sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 20 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > load1KB80GB_${today}_${mod}.txt 2>&1
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/raid0/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config_withoutWAL.ini > load1KB20GB_${today}_${mod}.txt 2>&1

# sleep 300

# echo "run start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 20 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB80GB_${today}_${mod}.txt 2>&1

mod="random-noWAL-40T-125M-2"
echo "load start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 20 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > load1KB80GB_${today}_${mod}.txt 2>&1
sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config_withoutWAL.ini > load1KB20GB_${today}_${mod}.txt 2>&1


# sleep 300

# echo "run start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/sdb/testRocksdb -threads 8 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB80GB_${today}_${mod}.txt 2>&1

# mod="my-noWAL-40T-125M"
# echo "load start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config_withoutWAL.ini > load1KB20GB_${today}_${mod}.txt 2>&1
# ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 20 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config_withoutWAL.ini > load1KB80GB_${today}_${mod}.txt 2>&1

# echo "run start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb run -dbPath /mnt/raid0/testRocksdb -threads 8 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config.ini > run1KB80GB_${today}_${mod}.txt 2>&1

# mod="originalLevel-noWAL-40T-125M"
# echo "load start"
# sudo cgexec -g blkio:test_write ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 40 -P ../workloads/workloada1KB20GB.spec -dbConfig ../db_config/rocksdb_config_withoutWAL.ini > load1KB20GB_${today}_${mod}.txt 2>&1
# ./ycsbc -db rocksdb load -dbPath /mnt/sdb/testRocksdb -threads 20 -P ../workloads/workloada1KB80GB.spec -dbConfig ../db_config/rocksdb_config_withoutWAL.ini > load1KB80GB_${today}_${mod}.txt 2>&1