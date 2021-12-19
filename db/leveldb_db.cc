//
//  leveldb_db.cc
//  YCSB-C
//

#include "leveldb_db.h"
#include "leveldb/cache.h"
#include "leveldb/filter_policy.h"
// #include "leveldb/flush_block_policy.h"
#include "utils/rocksdb_config.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

namespace ycsbc {
LevelDB::LevelDB(const char *dbPath, const string dbConfig) : noResult(0) {
  name_ = "leveldb";
  // get rocksdb config
  ConfigRocksDB config;
  if (dbConfig.empty()) {
    config.init("./RocksDBConfig/rocksdb_config.ini");
  } else {
    config.init(dbConfig);
  }
  disableWAL_ = config.getDisableWAL();
  int bloomBits = config.getBloomBits();
  size_t blockCache = config.getBlockCache();
  bool seekCompaction = config.getSeekCompaction();
  bool compression = config.getCompression();
  bool directIO = config.getDirectIO();
  size_t memtable = config.getMemtable();
  size_t maxFileSize = config.getMaxFileSize();
  // set optionssc
  leveldb::Options options;
  options.compression = leveldb::kNoCompression;
  options.max_open_files = 65535;
  // rocksdb::BlockBasedTableOptions bbto;
  // options.multi_path = true;
  // options.db_paths = {{"/home/colin/hub/YCSB-C-RocksDB/build/testDir/vol1",
  //                      (uint64_t)1 * 1024 * 1024 * 1024},
  //                     {"/home/colin/hub/YCSB-C-RocksDB/build/testDir/vol2",
  //                      (uint64_t)3 * 1024 * 1024 * 1024},
  //                     {"/home/colin/hub/YCSB-C-RocksDB/build/testDir/vol3",
  //                      (uint64_t)300 * 1024 * 1024 * 1024}};
  // options.statistics = rocksdb::CreateDBStatistics();
  // statistics = options.statistics;
  options.create_if_missing = true;
  options.write_buffer_size = memtable;
  options.max_file_size = maxFileSize;
  // options.target_file_size_base = 64 << 20;               // 64MB
  // options.max_file_size = 64 << 20; 默认2MB
  // options.max_write_buffer_number = config.getMemtableNum(); // 3个imm
  // options.force_consistency_checks = false; // 一致性检查 零时关闭
  // options.compaction_pri = rocksdb::kMinOverlappingRatio;
  /*if (config.getTiered()) {
    options.compaction_style = rocksdb::kCompactionStyleUniversal;
  }*/
  // options.max_background_jobs = config.getNumThreads();
  // options.disable_auto_compactions = config.getNoCompaction();
  // options.level_compaction_dynamic_level_bytes = true;
  // options.target_file_size_base = 8<<20;
  cerr << "write buffer size: " << options.write_buffer_size << endl;
  cerr << "sstable file size: " << options.max_file_size << endl;
  /*cerr << "num compaction trigger: "
       << options.level0_file_num_compaction_trigger << endl;
  cerr << "targe file size base: " << options.target_file_size_base << endl;
  cerr << "level size base: " << options.max_bytes_for_level_base << endl;*/
  if (!compression)
    options.compression = leveldb::kNoCompression;
  /*if (bloomBits > 0) {
    // bbto.filter_policy.reset(rocksdb::NewBloomFilterPolicy(bloomBits));
    options.filter_policy = leveldb::NewBloomFilterPolicy(bloomBits);
  }*/
  // bbto.block_cache = rocksdb::NewLRUCache(blockCache);
  if (blockCache > 0) {
    options.block_cache = leveldb::NewLRUCache(blockCache);
  } else {
    options.block_cache = nullptr;
  }

  // options.table_factory.reset(rocksdb::NewBlockBasedTableFactory(bbto));

  leveldb::Status s = leveldb::DB::Open(options, dbPath, &db_);
  if (!s.ok()) {
    cerr << "Can't open rocksdb " << dbPath << endl;
    exit(0);
  }

  //开启profiling
  // cout<<"开启profiling"<<endl;
  // rocksdb::SetPerfLevel(rocksdb::PerfLevel::kEnableTimeExceptForMutex);
  // rocksdb::get_perf_context()->Reset();
  // rocksdb::get_iostats_context()->Reset();

  cout << "\nbloom bits:" << bloomBits << "bits\ndirectIO:" << (bool)directIO
       << "\nseekCompaction:" << (bool)seekCompaction << endl;
}

int LevelDB::Read(const std::string &table, const std::string &key,
                  const std::vector<std::string> *fields,
                  std::vector<KVPair> &result) {
  string value;
  leveldb::Status s = db_->Get(leveldb::ReadOptions(), key, &value);
  if (s.ok())
    return DB::kOK;
  if (s.IsNotFound()) {
    noResult++;
    cout << noResult << "not found" << endl;
    return DB::kOK;
  } else {
    cerr << "read error" << endl;
    exit(0);
  }
}

int LevelDB::Scan(const std::string &table, const std::string &key, int len,
                  const std::vector<std::string> *fields,
                  std::vector<std::vector<KVPair>> &result) {
  auto it = db_->NewIterator(leveldb::ReadOptions());
  it->Seek(key);
  std::string val;
  std::string k;
  int i;
  int cnt = 0;
  for (i = 0; i < len && it->Valid(); i++) {
    k = it->key().ToString();
    val = it->value().ToString();
    it->Next();
    if (val.empty())
      cnt++;
  }
  if (i < len) {
    std::cout << " get " << i << " for length " << len << "." << std::endl;
    std::cerr << " get " << i << " for length " << len << "." << std::endl;
  }
  if (cnt > 0) {
    std::cout << cnt << "empty values" << std::endl;
  }
  delete it;
  return DB::kOK;
}

int LevelDB::Insert(const std::string &table, const std::string &key,
                    std::vector<KVPair> &values) {
  leveldb::Status s;
  auto wo = leveldb::WriteOptions();
  // wo.disableWAL = disableWAL_;
  for (KVPair &p : values) {
    s = db_->Put(wo, key, p.second);
    if (!s.ok()) {
      cerr << "insert error" << s.ToString() << "\n" << endl;
      exit(0);
    }
  }
  return DB::kOK;
}

int LevelDB::Update(const std::string &table, const std::string &key,
                    std::vector<KVPair> &values) {
  return Insert(table, key, values);
}

int LevelDB::Delete(const std::string &table, const std::string &key) {
  vector<DB::KVPair> values;
  return Insert(table, key, values);
}

void LevelDB::printStats() {

  // string stats;
  // db_->GetProperty("rocksdb.stats", &stats);
  // cout << stats << endl;
  // cout << "=========================== options.statistics "
  //         "=============================="
  //      << endl;
  // cout << statistics->ToString() << endl;

  // //输出perf & io
  // std::cout << "===================== perf/IO "
  //              "statistics================================"
  //           << std::endl;
  // std::cout << rocksdb::get_perf_context()->ToString() << std::endl;
  // std::cout << rocksdb::get_iostats_context()->ToString() << std::endl;
  // //关闭profiling
  // rocksdb::SetPerfLevel(rocksdb::PerfLevel::kDisable);

  string stats;
  // 输出统计信息
  db_->GetProperty("leveldb.stats", &stats);
  std::cout << "------------------------------------- leveldb.stats "
               "--------------------------------------"
            << std::endl;
  std::cout << stats << std::endl;

  // 输出各层数据

  db_->GetProperty("leveldb.sstables", &stats);
  std::cout << "------------------------------------- leveldb.sstables "
               "--------------------------------------"
            << std::endl;
  std::cout << stats << std::endl;
}

/*void EnablePerf() {
  rocksdb::SetPerfLevel(rocksdb::PerfLevel::kEnableTimeExceptForMutex);
  rocksdb::get_perf_context()->Reset();
  rocksdb::get_iostats_context()->Reset();
}*/

/*void DisablePerf() { rocksdb::SetPerfLevel(rocksdb::PerfLevel::kDisable); }*/

LevelDB::~LevelDB() { delete db_; }
} // namespace ycsbc
