#pragma once

#include "../../include/iterator/iterator.h"
#include "../skiplist/skiplist.h"
#include "memtable.h"
#include <queue>

class MemTableIterator : public BaseIterator {
public:
  MemTableIterator();
  MemTableIterator(const MemTable &memtable);
  std::pair<std::string, std::string> operator*() const;
  MemTableIterator &operator++();
  MemTableIterator operator++(int);
  bool operator==(const MemTableIterator &other) const;
  bool operator!=(const MemTableIterator &other) const;
  bool is_end() const;

private:
  std::priority_queue<SearchItem, std::vector<SearchItem>,
                      std::greater<SearchItem>>
      items;
};