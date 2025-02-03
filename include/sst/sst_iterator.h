#pragma once
#include "../iterator/iterator.h"
#include "../block/block_iterator.h"
#include <cstddef>
#include <memory>

class SST;

class SstIterator {
  friend SST;

private:
  std::shared_ptr<SST> m_sst;
  size_t m_block_idx;
  std::shared_ptr<BlockIterator> m_block_it;

public:
  using value_type = std::pair<std::string, std::string>;

  // 创建迭代器, 并移动到第一个key
  SstIterator(std::shared_ptr<SST> sst);
  // 创建迭代器, 并移动到第指定key
  SstIterator(std::shared_ptr<SST> sst, const std::string &key);

  void seek_first();
  void seek(const std::string &key);
  bool is_end();
  std::string key();
  std::string value();

  SstIterator &operator++();
  SstIterator operator++(int) = delete;
  bool operator==(const SstIterator &other) const;
  bool operator!=(const SstIterator &other) const;
  value_type operator*() const;
};