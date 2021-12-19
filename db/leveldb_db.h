#ifndef YCSB_C_LEVELDB_DB_H_
#define YCSB_C_LEVELDB_DB_H_

#include "core/db.h"
#include "core/properties.h"
#include "leveldb/db.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
// #include "leveldb/iostats_context.h"
#include "leveldb/options.h"
// #include "leveldb/perf_context.h"
// #include "leveldb/perf_level.h"
#include <iostream>
#include <string>

namespace ycsbc {

class LevelDB : public DB {
public:
  LevelDB(const char *dbPath, const std::string dbConfig);

  int Read(const std::string &table, const std::string &key,
           const std::vector<std::string> *fields, std::vector<KVPair> &result);

  int Scan(const std::string &table, const std::string &key, int len,
           const std::vector<std::string> *fields,
           std::vector<std::vector<KVPair>> &result);

  int Update(const std::string &table, const std::string &key,
             std::vector<KVPair> &values);

  int Insert(const std::string &table, const std::string &key,
             std::vector<KVPair> &values);

  int Delete(const std::string &table, const std::string &key);

  void EnablePerf(){};

  void DisablePerf(){};

  void printStats();

  ~LevelDB();

private:
  leveldb::DB *db_;
  unsigned noResult;
  bool disableWAL_;
  // std::shared_ptr<rocksdb::Statistics> statistics;
  leveldb::Iterator *it{nullptr};
};

} // namespace ycsbc

#endif // YCSB_C_LEVEL_DB_H_
