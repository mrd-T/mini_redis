#pragma once

#include "../../include/iterator/iterator.h"
#include "../../include/memtable/mem_iterator.h"
#include "../../include/sst/sst_iterator.h"

#include <memory>

class MergeIterator : public BaseIterator {
private:
  MemTableIterator it_a;
  SstIterator it_b;
  bool choose_a = false;

public:
  MergeIterator(MemTableIterator it_a, SstIterator it_b);
  bool choose_it_a();
  void skip_it_b();

  std::pair<std::string, std::string> operator*() const;
  MergeIterator &operator++();
  std::shared_ptr<BaseIterator> operator++(int);
  bool operator==(const MergeIterator &other) const;
  bool operator!=(const MergeIterator &other) const;
};
