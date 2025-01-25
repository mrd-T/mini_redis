#ifndef MEMTABLE_H
#define MEMTABLE_H

#include "../skiplist/skiplist.h"
#include <cstddef>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

class MemTable {
public:
  MemTable();
  ~MemTable();

  void put(const std::string &key, const std::string &value);
  std::optional<std::string> get(const std::string &key);
  void remove(const std::string &key);
  void clear();
  void flush();
  void frozen_cur_table();
  size_t get_cur_size() const;
  size_t get_frozen_size() const;
  size_t get_total_size() const;

private:
  std::shared_ptr<SkipList> current_table;
  std::list<std::shared_ptr<SkipList>> frozen_tables;
  size_t frozen_bytes;
};

#endif // MEMTABLE_H