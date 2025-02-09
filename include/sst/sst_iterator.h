#pragma once
#include "../block/block_iterator.h"
#include "../iterator/iterator.h"
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

class SstIterator;
class SST;

std::optional<std::pair<SstIterator, SstIterator>>
sst_iters_monotony_predicate(std::shared_ptr<SST> sst,
                             std::function<int(const std::string &)> predicate);

class SstIterator {
  friend std::optional<std::pair<SstIterator, SstIterator>>
  sst_iters_monotony_predicate(
      std::shared_ptr<SST> sst,
      std::function<int(const std::string &)> predicate);

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
  void set_block_idx(size_t idx);
  void set_block_it(std::shared_ptr<BlockIterator> it);

public:
  // 创建迭代器, 并移动到第一个key
  SstIterator(std::shared_ptr<SST> sst);
  // 创建迭代器, 并移动到第指定key
  SstIterator(std::shared_ptr<SST> sst, const std::string &key);

  // 创建迭代器, 并移动到第指定前缀的首端或者尾端
  static std::optional<std::pair<SstIterator, SstIterator>>
  iters_monotony_predicate(std::shared_ptr<SST> sst,
                           std::function<bool(const std::string &)> predicate);

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
  bool is_valid();
};