#pragma once

#include "../iterator/iterator.h"
#include "../sst/sst_iterator.h"

#include <memory>

class MergeIterator {
private:
  HeapIterator it_a;
  HeapIterator it_b;
  bool choose_a = false;

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
};
