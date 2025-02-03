#pragma once

#include "../iterator/iterator.h"
#include "../skiplist/skiplist.h"
#include <cstddef>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

class MemTable {
  friend class HeapIterator;

public:
  MemTable();
  ~MemTable();

  void put(const std::string &key, const std::string &value);
  std::optional<std::string> get(const std::string &key);
  void remove(const std::string &key);
  void clear();
  void flush();
  void frozen_cur_table();
  size_t get_cur_size();
  size_t get_frozen_size();
  size_t get_total_size();
  HeapIterator begin();
  HeapIterator end();

private:
  std::shared_ptr<SkipList> current_table;
  std::list<std::shared_ptr<SkipList>> frozen_tables;
  size_t frozen_bytes;
  std::shared_mutex rx_mtx; // 以整个 SkipList 为单位的锁
};