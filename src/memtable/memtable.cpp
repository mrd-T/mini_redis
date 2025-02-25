#include "../../include/memtable/memtable.h"
#include "../../include/consts.h"
#include "../../include/iterator/iterator.h"
#include "../../include/skiplist/skiplist.h"
#include "../../include/sst/sst.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <type_traits>
#include <utility>
#include <vector>

class BlockCache;

// MemTable implementation using PIMPL idiom
MemTable::MemTable() : frozen_bytes(0) {
  current_table = std::make_shared<SkipList>();
}
MemTable::~MemTable() = default;

void MemTable::put_(const std::string &key, const std::string &value) {
  current_table->put(key, value);

  if (current_table->get_size() > LSM_PER_MEM_SIZE_LIMIT) {
    frozen_cur_table_();
  }
}

void MemTable::put(const std::string &key, const std::string &value) {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  put_(key, value);
}

void MemTable::put_batch(
    const std::vector<std::pair<std::string, std::string>> &kvs) {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  for (auto &[k, v] : kvs) {
    put_(k, v);
  }
}

std::optional<std::string> MemTable::get_(const std::string &key) {
  // 首先检查当前活跃的memtable
  auto result = current_table->get(key);
  if (result.has_value()) {
    auto data = result.value();
    // 只要找到了 key, 不管 value 是否为空都返回
    return data;
  }

  // 如果当前memtable中没有找到，检查frozen memtable
  for (auto &tabe : frozen_tables) {
    auto result = tabe->get(key);
    if (result.has_value()) {
      return result;
    }
  }

  // 都没有找到，返回空
  return std::nullopt;
}

std::optional<std::string> MemTable::get(const std::string &key) {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  return get_(key);
}

void MemTable::remove_(const std::string &key) {
  // 删除的方式是写入空值
  current_table->put(key, "");
  if (current_table->get_size() > LSM_PER_MEM_SIZE_LIMIT) {
    frozen_cur_table_();
  }
}

void MemTable::remove(const std::string &key) {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  remove_(key);
}

void MemTable::remove_batch(const std::vector<std::string> &keys) {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  // 删除的方式是写入空值
  for (auto &key : keys) {
    remove_(key);
  }
}

void MemTable::clear() {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  frozen_tables.clear();
  current_table->clear();
}

// 将最老的 memtable 写入 SST, 并返回控制类
std::shared_ptr<SST>
MemTable::flush_last(SSTBuilder &builder, std::string &sst_path, size_t sst_id,
                     std::shared_ptr<BlockCache> block_cache) {
  // 由于 flush 后需要移除最老的 memtable, 因此需要加写锁
  std::unique_lock<std::shared_mutex> lock(rx_mtx);

  if (frozen_tables.empty()) {
    // 如果当前表为空，直接返回nullptr
    if (current_table->get_size() == 0) {
      return nullptr;
    }
    // 将当前表加入到frozen_tables头部
    frozen_tables.push_front(current_table);
    frozen_bytes += current_table->get_size();
    // 创建新的空表作为当前表
    current_table = std::make_shared<SkipList>();
  }

  // 将最老的 memtable 写入 SST
  std::shared_ptr<SkipList> table = frozen_tables.back();
  frozen_tables.pop_back();
  frozen_bytes -= table->get_size();

  std::vector<std::pair<std::string, std::string>> flush_data = table->flush();
  for (auto &[k, v] : flush_data) {
    builder.add(k, v);
  }
  auto sst = builder.build(sst_id, sst_path, block_cache);
  return sst;
}

void MemTable::frozen_cur_table_() {
  frozen_bytes += current_table->get_size();
  frozen_tables.push_front(std::move(current_table));
  current_table = std::make_shared<SkipList>();
}

void MemTable::frozen_cur_table() {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  frozen_cur_table_();
}

size_t MemTable::get_cur_size() {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  return current_table->get_size();
}

size_t MemTable::get_frozen_size() {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  return frozen_bytes;
}

size_t MemTable::get_total_size() {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  return get_frozen_size() + get_cur_size();
}

HeapIterator MemTable::begin() {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  std::vector<SearchItem> item_vec;

  for (auto iter = current_table->begin(); iter != current_table->end();
       iter++) {
    item_vec.emplace_back(iter.get_key(), iter.get_value(), 0);
  }

  int table_idx = 1;
  for (auto ft = frozen_tables.begin(); ft != frozen_tables.end(); ft++) {
    auto table = *ft;
    for (auto iter = table->begin(); iter != table->end(); iter++) {
      item_vec.emplace_back(iter.get_key(), iter.get_value(), table_idx);
    }
    table_idx++;
  }

  return HeapIterator(item_vec);
}

HeapIterator MemTable::end() {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  return HeapIterator{};
}

HeapIterator MemTable::iters_preffix(const std::string &preffix) {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  std::vector<SearchItem> item_vec;

  for (auto iter = current_table->begin_preffix(preffix);
       iter != current_table->end_preffix(preffix); iter++) {
    item_vec.emplace_back(iter.get_key(), iter.get_value(), 0);
  }

  int table_idx = 1;
  for (auto ft = frozen_tables.begin(); ft != frozen_tables.end(); ft++) {
    auto table = *ft;
    for (auto iter = table->begin_preffix(preffix);
         iter != table->end_preffix(preffix); iter++) {
      item_vec.emplace_back(iter.get_key(), iter.get_value(), table_idx);
    }
    table_idx++;
  }

  return HeapIterator(item_vec);
}

std::optional<std::pair<HeapIterator, HeapIterator>>
MemTable::iters_monotony_predicate(
    std::function<int(const std::string &)> predicate) {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  std::vector<SearchItem> item_vec;

  auto cur_result = current_table->iters_monotony_predicate(predicate);
  if (cur_result.has_value()) {
    auto [begin, end] = cur_result.value();
    for (auto iter = begin; iter != end; ++iter) {
      item_vec.emplace_back(iter.get_key(), iter.get_value(), 0);
    }
  }

  int table_idx = 1;
  for (auto ft = frozen_tables.begin(); ft != frozen_tables.end(); ft++) {
    auto table = *ft;
    auto result = table->iters_monotony_predicate(predicate);
    if (result.has_value()) {
      auto [begin, end] = result.value();
      for (auto iter = begin; iter != end; ++iter) {
        item_vec.emplace_back(iter.get_key(), iter.get_value(), table_idx);
      }
    }
    table_idx++;
  }

  if (item_vec.empty()) {
    return std::nullopt;
  }
  return std::make_pair(HeapIterator(item_vec), HeapIterator{});
}
