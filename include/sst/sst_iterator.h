#pragma once
#include "../block/block_iterator.h"
#include "../iterator/iterator.h"
#include <cstddef>
#include <memory>

class SST;

class SstIterator {
  friend SST;

  using value_type = std::pair<std::string, std::string>;
  using pointer = value_type *;
  using reference = value_type &;

private:
  std::shared_ptr<SST> m_sst;
  size_t m_block_idx;
  std::shared_ptr<BlockIterator> m_block_it;
  mutable std::optional<value_type> cached_value; // 缓存当前值

  void update_current() const;

public:
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
  pointer operator->() const;
};