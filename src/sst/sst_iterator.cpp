#include "../../include/sst/sst_iterator.h"
#include "../../include/sst/sst.h"
#include <stdexcept>

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
    auto block = m_sst->read_block(m_block_idx);
    if (!block) {
      m_block_it = nullptr;
      return;
    }
    m_block_it = std::make_shared<BlockIterator>(block, key);
  } catch (const std::exception &) {
    m_block_it = nullptr;
    throw std::runtime_error("could not read a block from m_sst");
  }
}

bool SstIterator::is_end() { return !m_block_it; }

std::string SstIterator::key() {
  if (!m_block_it) {
    throw std::runtime_error("Iterator is invalid");
  }
  return (*m_block_it)->second;
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