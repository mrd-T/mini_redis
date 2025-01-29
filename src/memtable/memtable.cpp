#include "../../include/memtable/memtable.h"
#include "../../include/consts.h"
#include "../../include/memtable/iterator.h"
#include "../../include/skiplist/skiplist.h"
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <type_traits>
#include <utility>

// MemTable implementation using PIMPL idiom
MemTable::MemTable() : frozen_bytes(0) {
  current_table = std::make_shared<SkipList>();
}
MemTable::~MemTable() = default;

void MemTable::put(const std::string &key, const std::string &value) {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  current_table->put(key, value);
}

std::optional<std::string> MemTable::get(const std::string &key) {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  // 首先检查当前活跃的memtable
  auto result = current_table->get(key);
  if (result.has_value()) {
    auto data = result.value();
    if (!data.empty()) {
      return data;
    } else {
      // 空值表示被删除了
      return std::nullopt;
    }
  }

  // 如果当前memtable中没有找到，检查frozen memtable
  for (auto &tabe : frozen_tables) {
    auto result = tabe->get(key);
    if (result.has_value()) {
      auto data = result.value();
      if (!data.empty()) {
        return data;
      } else {
        // 空值表示被删除了
        return std::nullopt;
      }
    }
  }

  // 都没有找到，返回空
  // !!! 目前只实现了内存中的 memtable, 还没有实现 SST
  return std::nullopt;
}

void MemTable::remove(const std::string &key) {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  // 删除的方式是写入空值
  current_table->put(key, "");
}

void MemTable::clear() {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  frozen_tables.clear();
  current_table->clear();
}

void MemTable::flush() {
  // TODO: 将数据写入SSTable
}

void MemTable::frozen_cur_table() {
  std::unique_lock<std::shared_mutex> lock(rx_mtx);
  frozen_bytes += current_table->get_size();
  frozen_tables.push_front(std::move(current_table));
  current_table = std::make_shared<SkipList>();
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

MemTableIterator MemTable::begin() {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  return MemTableIterator(*this);
}

MemTableIterator MemTable::end() {
  std::shared_lock<std::shared_mutex> slock(rx_mtx);
  return MemTableIterator{};
}
