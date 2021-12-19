//
//  basic_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/db_factory.h"

#include "db/basic_db.h"
#include "db/leveldb_db.h"
// #include "db/lock_stl_db.h"
#include "db/rocksdb_db.h"
// #include "db/tbb_rand_db.h"
// #include "db/tbb_scan_db.h"
#include <string>

using namespace std;
using ycsbc::DB;
using ycsbc::DBFactory;

shared_ptr<DB> DBFactory::CreateDB(utils::Properties &props) {
  if (props["dbname"] == "basic") {
    return make_shared<BasicDB>();
    /*} else if (props["dbname"] == "lock_stl") {
      return make_shared<LockStlDB>();
    } else if (props["dbname"] == "tbb_rand") {
      return make_shared<TbbRandDB>();
    } else if (props["dbname"] == "tbb_scan") {
      return make_shared<TbbScanDB>();*/
  } else if (props["dbname"] == "rocksdb") {
    return make_shared<RocksDB>(props["dbPath"].c_str(),
                                props.GetProperty("dbConfig", ""));
  } else if (props["dbname"] == "leveldb") {
    return make_shared<LevelDB>(props["dbPath"].c_str(),
                                props.GetProperty("dbConfig", ""));
  } else
    return NULL;
}
