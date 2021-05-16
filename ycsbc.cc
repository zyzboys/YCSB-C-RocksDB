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
#include "utils/histogram.h"
#include <cstring>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

void UsageMessage(const char *command);
bool StrStartWith(const char *str, const char *pre);
string ParseCommandLine(int argc, const char *argv[], utils::Properties &props);

int DelegateClient(shared_ptr<ycsbc::DB> db, ycsbc::CoreWorkload *wl,
                   const int num_ops, bool is_loading,
                   shared_ptr<ycsbc::Histogram> his) {
  db->Init();
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

  vector<future<int>> actual_ops;
  vector<shared_ptr<ycsbc::Histogram>> histograms;
  utils::Timer<utils::t_seconds> timer;

  string pattern = props.GetProperty("pattern");

  // Loads data
  if (pattern == "load" || pattern == "both") {
    int total_ops = stoi(props[ycsbc::CoreWorkload::RECORD_COUNT_PROPERTY]);
    timer.Start();
    for (int i = 0; i < num_threads; ++i) {
      shared_ptr<ycsbc::Histogram> his = make_shared<ycsbc::Histogram>();
      his->Clear();
      histograms.emplace_back(his);
      actual_ops.emplace_back(async(launch::async, DelegateClient, db, &wl,
                                    total_ops / num_threads, true, his));
    }
    assert((int)actual_ops.size() == num_threads);

    int sum = 0;
    for (auto &n : actual_ops) {
      assert(n.valid());
      sum += n.get();
    }
    auto duration = timer.End();
    for (int i = 1; i < num_threads; i++) {
      histograms[0]->Merge(*histograms[i]);
    }
    cerr << "# Loading records:\t" << sum << endl;
    cerr << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t';
    cerr << total_ops / duration << " OPS" << endl;
    cerr << histograms[0]->ToString() << endl;
    if(props.GetProperty("dbname")=="rocksdb"){
      cerr<<"============================statistics==========================="<<endl;
      db->printStats();
    }
  }

  // Peforms transactions
  if (pattern == "run" || pattern == "both") {
    actual_ops.clear();
    histograms.clear();
    int total_ops = stoi(props[ycsbc::CoreWorkload::OPERATION_COUNT_PROPERTY]);
    timer.Start();
    for (int i = 0; i < num_threads; ++i) {
      shared_ptr<ycsbc::Histogram> his = make_shared<ycsbc::Histogram>();
      histograms.emplace_back(his);
      actual_ops.emplace_back(async(launch::async, DelegateClient, db, &wl,
                                    total_ops / num_threads, false, his));
    }
    assert((int)actual_ops.size() == num_threads);

    int sum = 0;
    for (auto &n : actual_ops) {
      assert(n.valid());
      sum += n.get();
    }
    auto duration = timer.End();
    for (int i = 1; i < num_threads; i++) {
      histograms[0]->Merge(*histograms[i]);
    }
    cerr << "# Transaction throughput " << endl;
    cerr << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t';
    cerr << total_ops / duration << " OPS" << endl;
    cerr << histograms[0]->ToString() << endl;
    if(props.GetProperty("dbname")=="rocksdb"){
      cerr<<"============================statistics==========================="<<endl;
      db->printStats();
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
    }else if (strcmp(argv[argindex], "-dbConfig") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("dbConfig", argv[argindex]);
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
    cout<<"Unknown pattern"<<endl;
    UsageMessage(argv[0]);
    exit(0);
  }

  return filename;
}

void UsageMessage(const char *command) {
  cout << "Usage: " << command << " [options]" << endl;
  cout << "Options:" << endl;
  cout << "  pattern: load/run/both"<< endl;
  cout << "  load: load the database from file" << endl;
  
  cout << "  run: peforms transactions in the previous database" << endl;
  cout << "  both: load and run" << endl;
  cout << "  'load','run','both' must choose one" << endl;
  cout << "  -threads n: execute using n threads (default: 1)" << endl;
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
