//
//  ycsbc.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "core/client.h"
#include "core/core_workload.h"
#include "core/timer.h"
#include "core/utils.h"
#include "db/db_factory.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/perf_context.h"
#include "utils/histogram.h"
#include <chrono>
#include <cstring>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace std;

void UsageMessage(const char *command);
bool StrStartWith(const char *str, const char *pre);
string ParseCommandLine(int argc, const char *argv[], utils::Properties &props);

void mergePerf(shared_ptr<rocksdb::PerfContext> a,
               shared_ptr<rocksdb::PerfContext> b) {
  a->write_memtable_time += b->write_memtable_time;
  a->write_wal_time += b->write_wal_time;
  a->write_pre_and_post_process_time += b->write_pre_and_post_process_time;
  a->write_scheduling_flushes_compactions_time +=
      b->write_scheduling_flushes_compactions_time;
  a->write_delay_time += b->write_delay_time;
  a->write_thread_wait_nanos += b->write_thread_wait_nanos;
}

int record(shared_ptr<ycsbc::DB> db, atomic<bool> *isFinish, int duration) {
  while (!(*isFinish)) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // sleep 1s
    std::this_thread::sleep_for(std::chrono::seconds(duration)); // sleep 10s
    db->printStats();
  }
  return 0;
}
int DelegateClient(shared_ptr<ycsbc::DB> db, ycsbc::CoreWorkload *wl,
                   const int num_ops, bool is_loading,
                   shared_ptr<ycsbc::Histogram> his,
                   shared_ptr<rocksdb::PerfContext> perf_Context,
                   shared_ptr<rocksdb::IOStatsContext> iostats_Context) {
  db->Init();
  // db->EnablePerf();
  rocksdb::SetPerfLevel(rocksdb::PerfLevel::kEnableTimeExceptForMutex);
  rocksdb::get_perf_context()->Reset();
  rocksdb::get_iostats_context()->Reset();

  ycsbc::Client client(*db, *wl);
  utils::Timer<utils::t_microseconds> timer;
  int oks = 0;
  for (int i = 0; i < num_ops; ++i) {
    timer.Start();
    if (is_loading) {
      oks += client.DoInsert();
    } else {
      oks += client.DoTransaction();
    }
    double duration = timer.End();
    his->AddFast(duration);
  }
  db->DisablePerf();
  *perf_Context = *rocksdb::get_perf_context();
  *iostats_Context = *rocksdb::get_iostats_context();
  db->Close();
  return oks;
}

int main(const int argc, const char *argv[]) {
  utils::Properties props;
  string file_name = ParseCommandLine(argc, argv, props);

  shared_ptr<ycsbc::DB> db = ycsbc::DBFactory::CreateDB(props);
  if (!db) {
    cout << "Unknown database name " << props["dbname"] << endl;
    exit(0);
  }

  ycsbc::CoreWorkload wl;
  wl.Init(props);

  const int num_threads = stoi(props.GetProperty("threadcount", "1"));
  atomic<bool> *isFinish = new atomic<bool>(false);

  vector<future<int>> actual_ops;
  vector<shared_ptr<ycsbc::Histogram>> histograms;
  vector<shared_ptr<rocksdb::PerfContext>> perfs;
  vector<shared_ptr<rocksdb::IOStatsContext>> ioStatss;
  utils::Timer<utils::t_seconds> timer;

  string pattern = props.GetProperty("pattern");

  int duration = stoi(props.GetProperty("duration", "0"));
  if (duration) {
    auto r = async(launch::async, record, db, isFinish, duration);
  }
  // thread recordT(record,isFinish);
  // recordT.detach();

  // Loads data
  if (pattern == "load" || pattern == "both") {
    int total_ops = stoi(props[ycsbc::CoreWorkload::RECORD_COUNT_PROPERTY]);
    timer.Start();
    for (int i = 0; i < num_threads; ++i) {
      shared_ptr<ycsbc::Histogram> his = make_shared<ycsbc::Histogram>();
      shared_ptr<rocksdb::PerfContext> perf_Context =
          make_shared<rocksdb::PerfContext>();
      shared_ptr<rocksdb::IOStatsContext> iostats_Context =
          make_shared<rocksdb::IOStatsContext>();
      his->Clear();
      histograms.emplace_back(his);
      perfs.emplace_back(perf_Context);
      ioStatss.emplace_back(iostats_Context);
      actual_ops.emplace_back(async(launch::async, DelegateClient, db, &wl,
                                    total_ops / num_threads, true, his,
                                    perf_Context, iostats_Context));
    }
    assert((int)actual_ops.size() == num_threads);

    int sum = 0;
    for (auto &n : actual_ops) {
      assert(n.valid());
      sum += n.get();
    }
    *isFinish = true; // finish tasks
    // r.get();
    delete isFinish;
    auto duration = timer.End();
    for (int i = 1; i < num_threads; i++) {
      histograms[0]->Merge(*histograms[i]);
    }
    shared_ptr<rocksdb::PerfContext> perf_Context =
        make_shared<rocksdb::PerfContext>();
    for (int i = 0; i < num_threads; i++) {
      mergePerf(perf_Context, perfs[i]);
    }
    cerr << "# Loading records:\t" << sum << endl;
    cerr << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t';
    cerr << total_ops / duration << " OPS" << endl;
    cerr << histograms[0]->ToString() << endl;
    if (props.GetProperty("dbname") == "rocksdb") {
      cerr
          << "============================statistics==========================="
          << endl;
      db->printStats();
      cerr << "============================ perf/io "
              "statistics==========================="
           << endl;
      cerr << perf_Context->ToString() << endl;
    }
  }

  // Peforms transactions
  if (pattern == "run" || pattern == "both") {
    actual_ops.clear();
    histograms.clear();
    perfs.clear();
    ioStatss.clear();
    int total_ops = stoi(props[ycsbc::CoreWorkload::OPERATION_COUNT_PROPERTY]);
    timer.Start();
    for (int i = 0; i < num_threads; ++i) {
      shared_ptr<ycsbc::Histogram> his = make_shared<ycsbc::Histogram>();
      shared_ptr<rocksdb::PerfContext> perf_Context =
          make_shared<rocksdb::PerfContext>();
      shared_ptr<rocksdb::IOStatsContext> iostats_Context =
          make_shared<rocksdb::IOStatsContext>();
      his->Clear();
      perfs.emplace_back(perf_Context);
      histograms.emplace_back(his);
      actual_ops.emplace_back(async(launch::async, DelegateClient, db, &wl,
                                    total_ops / num_threads, false, his,
                                    perf_Context, iostats_Context));
    }
    assert((int)actual_ops.size() == num_threads);

    int sum = 0;
    for (auto &n : actual_ops) {
      assert(n.valid());
      sum += n.get();
    }
    *isFinish = true; // finish tasks
    delete isFinish;
    auto duration = timer.End();
    for (int i = 1; i < num_threads; i++) {
      histograms[0]->Merge(*histograms[i]);
    }
    shared_ptr<rocksdb::PerfContext> perf_Context =
        make_shared<rocksdb::PerfContext>();
    for (int i = 0; i < num_threads; i++) {
      mergePerf(perf_Context, perfs[i]);
    }
    cerr << "# Transaction throughput " << endl;
    cerr << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t';
    cerr << total_ops / duration << " OPS" << endl;
    cerr << histograms[0]->ToString() << endl;
    if (props.GetProperty("dbname") == "rocksdb") {
      cerr
          << "============================statistics==========================="
          << endl;
      db->printStats();

      cerr << "============================ perf/io "
              "statistics==========================="
           << endl;
      cerr << perf_Context->ToString() << endl;
    }
  }
}

string ParseCommandLine(int argc, const char *argv[],
                        utils::Properties &props) {
  int argindex = 1;
  string filename;
  // while (argindex < argc && StrStartWith(argv[argindex], "-")) {
  while (argindex < argc) {
    if (strcmp(argv[argindex], "-threads") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("threadcount", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "load") == 0) {
      props.SetProperty("pattern", "load");
      argindex++;
    } else if (strcmp(argv[argindex], "run") == 0) {
      props.SetProperty("pattern", "run");
      argindex++;
    } else if (strcmp(argv[argindex], "both") == 0) {
      props.SetProperty("pattern", "both");
      argindex++;
    } else if (strcmp(argv[argindex], "-db") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("dbname", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-dbPath") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("dbPath", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-dbConfig") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("dbConfig", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-d") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("duration", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-host") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("host", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-port") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("port", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-slaves") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("slaves", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-P") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      filename.assign(argv[argindex]);
      ifstream input(argv[argindex]);
      try {
        props.Load(input);
      } catch (const string &message) {
        cout << message << endl;
        exit(0);
      }
      input.close();
      argindex++;
    } else {
      cout << "Unknown option '" << argv[argindex] << "'" << endl;
      exit(0);
    }
  }

  if (argindex == 1 || argindex != argc) {
    UsageMessage(argv[0]);
    exit(0);
  }

  string pattern = props.GetProperty("pattern");
  if (pattern.empty()) {
    cout << "Unknown pattern" << endl;
    UsageMessage(argv[0]);
    exit(0);
  }

  return filename;
}

void UsageMessage(const char *command) {
  cout << "Usage: " << command << " [options]" << endl;
  cout << "Options:" << endl;
  cout << "  pattern: load/run/both" << endl;
  cout << "  load: load the database from file" << endl;

  cout << "  run: peforms transactions in the previous database" << endl;
  cout << "  both: load and run" << endl;
  cout << "  'load','run','both' must choose one" << endl;
  cout << "  -threads n: execute using n threads (default: 1)" << endl;
  cout << "  -d n: print DB stats in n seconds (default: 0,mean do not print "
          "stats info)"
       << endl;
  cout << "  -db dbname: specify the name of the DB to use (default: basic)"
       << endl;
  cout << "  -P propertyfile: load properties from the given file. Multiple "
          "files can"
       << endl;
  cout << "                   be specified, and will be processed in the order "
          "specified"
       << endl;
}

inline bool StrStartWith(const char *str, const char *pre) {
  return strncmp(str, pre, strlen(pre)) == 0;
}
