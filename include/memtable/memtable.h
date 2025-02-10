#pragma once

#include "../iterator/iterator.h"
#include "../skiplist/skiplist.h"
#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>

class BlockCache;
class SST;
class SSTBuilder;

class MemTable {
  friend class HeapIterator;

public:
  MemTable();
  ~MemTable();

  void put(const std::string &key, const std::string &value);
  void put_batch(const std::vector<std::pair<std::string, std::string>> &kvs);

  std::optional<std::string> get(const std::string &key);
  void remove(const std::string &key);
  void remove_batch(const std::vector<std::string> &keys);

  void clear();
  std::shared_ptr<SST> flush_last(SSTBuilder &builder, std::string &sst_path,
                                  size_t sst_id,
                                  std::shared_ptr<BlockCache> block_cache);
  void frozen_cur_table();
  size_t get_cur_size();
  size_t get_frozen_size();
  size_t get_total_size();
  HeapIterator begin();
  HeapIterator iters_preffix(const std::string &preffix);

  std::optional<std::pair<HeapIterator, HeapIterator>>
  iters_monotony_predicate(std::function<int(const std::string &)> predicate);

  HeapIterator end();

private:
  std::shared_ptr<SkipList> current_table;
  std::list<std::shared_ptr<SkipList>> frozen_tables;
  size_t frozen_bytes;
  std::shared_mutex rx_mtx; // 以整个 SkipList 为单位的锁
};