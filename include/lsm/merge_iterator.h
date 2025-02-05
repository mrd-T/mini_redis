#pragma once

#include "../iterator/iterator.h"
#include "../sst/sst_iterator.h"

#include <memory>

class MergeIterator {
  using value_type = std::pair<std::string, std::string>;
  using pointer = value_type *;

private:
  HeapIterator it_a;
  HeapIterator it_b;
  bool choose_a = false;
  mutable std::shared_ptr<value_type> current; // 用于存储当前元素

  void update_current() const;

public:
  MergeIterator();
  MergeIterator(HeapIterator it_a, HeapIterator it_b);
  bool choose_it_a();
  void skip_it_b();
  bool is_end() const;

  std::pair<std::string, std::string> operator*() const;
  MergeIterator &operator++();
  bool operator==(const MergeIterator &other) const;
  bool operator!=(const MergeIterator &other) const;
  pointer operator->() const;
};
