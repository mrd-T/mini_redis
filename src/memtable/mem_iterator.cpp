#include "../../include/memtable/mem_iterator.h"
#include <vector>

MemTableIterator::MemTableIterator() {}

MemTableIterator::MemTableIterator(const MemTable &memtable) {
  auto cur_table = memtable.current_table;

  for (auto iter = cur_table->begin(); iter != cur_table->end(); iter++) {
    items.push(SearchItem{iter.get_key(), iter.get_value(), 0});
  }

  int level = 1;
  for (auto ft = memtable.frozen_tables.begin();
       ft != memtable.frozen_tables.end(); ft++) {
    auto table = *ft;
    for (auto iter = table->begin(); iter != table->end(); iter++) {
      items.push(SearchItem{iter.get_key(), iter.get_value(), level});
    }
    level++;
  }

  while (!items.empty() && items.top().value.empty()) {
    // 如果当前元素的value为空，则说明该元素已经被删除，需要从优先队列中删除
    auto del_key = items.top().key;
    while (!items.empty() && items.top().key == del_key) {
      items.pop();
    }
  }
}

std::pair<std::string, std::string> MemTableIterator::operator*() const {
  return std::make_pair(items.top().key, items.top().value);
}

MemTableIterator &MemTableIterator::operator++() {
  if (items.empty()) {
    return *this; // 处理空队列情况
  }

  auto old_item = items.top();
  items.pop();

  // 删除与旧元素key相同的元素
  while (!items.empty() && items.top().key == old_item.key) {
    items.pop();
  }

  // 处理被删除的元素
  while (!items.empty() && items.top().value.empty()) {
    auto del_key = items.top().key;
    while (!items.empty() && items.top().key == del_key) {
      items.pop();
    }
  }

  return *this;
}

MemTableIterator MemTableIterator::operator++(int) {
  MemTableIterator temp = *this;
  ++(*this);
  return temp;
}

bool MemTableIterator::operator==(const MemTableIterator &other) const {
  if (items.empty() && other.items.empty()) {
    return true;
  }
  if (items.empty() || other.items.empty()) {
    return false;
  }
  return items.top().key == other.items.top().key &&
         items.top().value == other.items.top().value;
}

bool MemTableIterator::operator!=(const MemTableIterator &other) const {
  return !(*this == other);
}

bool MemTableIterator::is_end() const { return items.empty(); }
