#include "../../include/sst/sst_iterator.h"
#include "../../include/sst/sst.h"
#include <cstddef>
#include <optional>
#include <stdexcept>

// predicate返回值:
//   0: 谓词
//   >0: 不满足谓词, 需要向右移动
//   <0: 不满足谓词, 需要向左移动
std::optional<std::pair<SstIterator, SstIterator>> sst_iters_monotony_predicate(
    std::shared_ptr<SST> sst,
    std::function<int(const std::string &)> predicate) {
  std::optional<SstIterator> final_begin = std::nullopt;
  std::optional<SstIterator> final_end = std::nullopt;
  for (int block_idx = 0; block_idx < sst->meta_entries.size(); block_idx++) {
    auto block = sst->read_block(block_idx);

    BlockMeta &meta_i = sst->meta_entries[block_idx];
    if (predicate(meta_i.first_key) < 0 || predicate(meta_i.last_key) > 0) {
      break;
    }

    auto result_i = block->get_monotony_predicate_iters(predicate);
    if (result_i.has_value()) {
      auto [i_begin, i_end] = result_i.value();
      if (!final_begin.has_value()) {
        auto tmp_it = SstIterator(sst);
        tmp_it.set_block_idx(block_idx);
        tmp_it.set_block_it(i_begin);
        final_begin = tmp_it;
      }
      auto tmp_it = SstIterator(sst);
      tmp_it.set_block_idx(block_idx);
      tmp_it.set_block_it(i_end);
      if (tmp_it.is_end() && tmp_it.m_block_idx == sst->num_blocks()) {
        tmp_it.set_block_it(nullptr);
      }
      final_end = tmp_it;
    }
  }
  if (!final_begin.has_value() || !final_end.has_value()) {
    return std::nullopt;
  }
  return std::make_pair(final_begin.value(), final_end.value());
}

SstIterator::SstIterator(std::shared_ptr<SST> sst)
    : m_sst(sst), m_block_idx(0), m_block_it(nullptr) {
  if (m_sst) {
    seek_first();
  }
}

SstIterator::SstIterator(std::shared_ptr<SST> sst, const std::string &key)
    : m_sst(sst), m_block_idx(0), m_block_it(nullptr) {
  if (m_sst) {
    seek(key);
  }
}

void SstIterator::set_block_idx(size_t idx) { m_block_idx = idx; }
void SstIterator::set_block_it(std::shared_ptr<BlockIterator> it) {
  m_block_it = it;
}

void SstIterator::seek_first() {
  if (!m_sst || m_sst->num_blocks() == 0) {
    m_block_it = nullptr;
    return;
  }

  m_block_idx = 0;
  auto block = m_sst->read_block(m_block_idx);
  m_block_it = std::make_shared<BlockIterator>(block);
}

void SstIterator::seek(const std::string &key) {
  if (!m_sst) {
    m_block_it = nullptr;
    return;
  }

  try {
    m_block_idx = m_sst->find_block_idx(key);
    if (m_block_idx == -1 || m_block_idx >= m_sst->num_blocks()) {
      // 置为 end
      // TODO: 这个边界情况需要添加单元测试
      m_block_it = nullptr;
      return;
    }
    auto block = m_sst->read_block(m_block_idx);
    if (!block) {
      m_block_it = nullptr;
      return;
    }
    m_block_it = std::make_shared<BlockIterator>(block, key);
    if (m_block_it->is_end()) {
      // block 中找不到
      m_block_idx = m_sst->num_blocks();
      m_block_it = nullptr;
      return;
    }
  } catch (const std::exception &) {
    m_block_it = nullptr;
    return;
  }
}

bool SstIterator::is_end() { return !m_block_it; }

std::string SstIterator::key() {
  if (!m_block_it) {
    throw std::runtime_error("Iterator is invalid");
  }
  return (*m_block_it)->first;
}

std::string SstIterator::value() {
  if (!m_block_it) {
    throw std::runtime_error("Iterator is invalid");
  }
  return (*m_block_it)->second;
}

SstIterator &SstIterator::operator++() {
  if (!m_block_it) { // 添加空指针检查
    return *this;
  }
  ++(*m_block_it);
  if (m_block_it->is_end()) {
    m_block_idx++;
    if (m_block_idx < m_sst->num_blocks()) {
      // 读取下一个block
      auto next_block = m_sst->read_block(m_block_idx);
      BlockIterator new_blk_it(next_block, 0);
      (*m_block_it) = new_blk_it;
    } else {
      // 没有下一个block
      m_block_it = nullptr;
    }
  }
  return *this;
}

bool SstIterator::is_valid() {
  return m_block_it && !m_block_it->is_end() &&
         m_block_idx < m_sst->num_blocks();
}

bool SstIterator::operator==(const SstIterator &other) const {
  if (m_sst != other.m_sst || m_block_idx != other.m_block_idx) {
    return false;
  }

  if (!m_block_it && !other.m_block_it) {
    return true;
  }

  if (!m_block_it || !other.m_block_it) {
    return false;
  }

  return *m_block_it == *other.m_block_it;
}

bool SstIterator::operator!=(const SstIterator &other) const {
  return !(*this == other);
}

SstIterator::value_type SstIterator::operator*() const {
  if (!m_block_it) {
    throw std::runtime_error("Iterator is invalid");
  }
  return (**m_block_it);
}

SstIterator::pointer SstIterator::operator->() const {
  update_current();
  return &(*cached_value);
}

void SstIterator::update_current() const {
  if (!cached_value && m_block_it && !m_block_it->is_end()) {
    cached_value = *(*m_block_it);
  }
}