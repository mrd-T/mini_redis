#include "../../include/sst/block_iterator.h"
#include "../../include/sst/block.h"
#include <memory>
#include <stdexcept>

class Block;

BlockIterator::BlockIterator(std::shared_ptr<Block> b, size_t index)
    : block(b), current_index(index), cached_value(std::nullopt) {}

BlockIterator &BlockIterator::operator++() {
  if (block && current_index < block->size()) {
    ++current_index;
    cached_value = std::nullopt; // 清除缓存
  }
  return *this;
}

BlockIterator BlockIterator::operator++(int) {
  BlockIterator tmp = *this;
  ++(*this);
  return tmp;
}

bool BlockIterator::operator==(const BlockIterator &other) const {
  if (block == nullptr && other.block == nullptr) {
    return true;
  }
  if (block == nullptr || other.block == nullptr) {
    return false;
  }
  return block == other.block && current_index == other.current_index;
}

bool BlockIterator::operator!=(const BlockIterator &other) const {
  return !(*this == other);
}

BlockIterator::value_type BlockIterator::operator*() const {
  if (!block || current_index >= block->size()) {
    throw std::out_of_range("Iterator out of range");
  }

  // 使用缓存避免重复解析
  if (!cached_value) {
    size_t offset = block->get_offset_at(current_index);
    cached_value =
        std::make_pair(block->get_key_at(offset), block->get_value_at(offset));
  }
  return *cached_value;
}