#/bin/bash
ulimit -c unlimited
ulimit -n 65535
ssd="/mnt/sdb"
echo ${ssd}
sudo rm -rf ${ssd}/testRocksdb
sudo cp -r ${ssd}/testRocksdbMultiBackup/* ${ssd}/testRocksdb