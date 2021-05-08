# version of cmake
cmake_minimum_required(VERSION 3.10)

# define the name of project
project(ycsbc LANGUAGES CXX)

#specify c++ compiler version
set(CMAKE_CXX_STANDARD 11)

add_executable(ycsbc "")

target_compile_options(ycsbc PRIVATE -g -Wall -pthread)

target_sources(ycsbc
  PRIVATE
    "ycsbc.cc"
    "core/core_workload.cc"
    "db/db_factory.cc"
    "db/hashtable_db.cc"
    "db/rocksdb_db.cc"
)

target_include_directories(ycsbc
  PRIVATE
  ${PROJECT_SOURCE_DIR}
)

target_link_libraries(ycsbc
  pthread
  tbb
  ${PROJECT_SOURCE_DIR}/librocksdb.a
  zstd
  lz4
  bz2
  z
  snappy
  dl
  )
