#pragma once

#include "../../include/iterator/iterator.h"
#include "sst_iterator.h"
#include <queue>
#include <vector>

class L0Iterator {
public:
  L0Iterator(std::vector<SstIterator> its);
  bool is_end() const;
  std::pair<std::string, std::string> operator*() const;
  L0Iterator &operator++();
  L0Iterator operator++(int);
  bool operator==(const L0Iterator &other) const;
  bool operator!=(const L0Iterator &other) const;

private:
  size_t m_idx;
  //   std::priority_queue<typename Tp>;
};