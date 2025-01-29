#include "../../include/sst/block.h"
#include "../../include/sst/block_iterator.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>

std::vector<uint8_t> Block::encode() {
  // 计算总大小：数据段 + 偏移数组(每个偏移2字节) + 元素个数(2字节)
  size_t total_bytes = data.size() * sizeof(uint8_t) +
                       offsets.size() * sizeof(uint16_t) + sizeof(uint16_t);
  std::vector<uint8_t> encoded(total_bytes, 0);

  // 1. 复制数据段
  memcpy(encoded.data(), data.data(), data.size() * sizeof(uint8_t));

  // 2. 复制偏移数组
  size_t offset_pos = data.size() * sizeof(uint8_t);
  memcpy(encoded.data() + offset_pos,
         offsets.data(),                   // vector 的连续内存起始位置
         offsets.size() * sizeof(uint16_t) // 总字节数
  );

  // 3. 写入元素个数
  size_t num_pos =
      data.size() * sizeof(uint8_t) + offsets.size() * sizeof(uint16_t);
  uint16_t num_elements = offsets.size();
  memcpy(encoded.data() + num_pos, &num_elements, sizeof(uint16_t));

  return encoded;
}

std::shared_ptr<Block> Block::decode(const std::vector<uint8_t>& encoded) {
    // 使用 make_shared 创建对象
    auto block = std::make_shared<Block>();
    
    // 1. 安全性检查
    if (encoded.size() < sizeof(uint16_t)) {
        throw std::runtime_error("Encoded data too small");
    }

    // 2. 读取元素个数
    uint16_t num_elements;
    size_t num_elements_pos = encoded.size() - sizeof(uint16_t);
    memcpy(&num_elements, encoded.data() + num_elements_pos, sizeof(uint16_t));

    // 3. 验证数据大小
    size_t required_size = sizeof(uint16_t) + num_elements * sizeof(uint16_t);
    if (encoded.size() < required_size) {
        throw std::runtime_error("Invalid encoded data size");
    }

    // 4. 计算各段位置
    size_t offsets_section_start =
        num_elements_pos - num_elements * sizeof(uint16_t);

    // 5. 读取偏移数组
    block->offsets.resize(num_elements);
    memcpy(block->offsets.data(), encoded.data() + offsets_section_start,
           num_elements * sizeof(uint16_t));

    // 6. 复制数据段
    block->data.reserve(offsets_section_start); // 优化内存分配
    block->data.assign(encoded.begin(), encoded.begin() + offsets_section_start);

    return block;
}

std::string Block::get_first_key() {
  if (data.empty() || offsets.empty()) {
    return "";
  }

  // 读取第一个key的长度（前2字节）
  uint16_t key_len;
  memcpy(&key_len, data.data(), sizeof(uint16_t));

  // 读取key
  std::string key(reinterpret_cast<char *>(data.data() + sizeof(uint16_t)),
                  key_len);
  return key;
}

size_t Block::get_offset_at(size_t idx) const {
  if (idx > offsets.size()) {
    throw std::runtime_error("idx out of offsets range");
  }
  return offsets[idx];
}

void Block::add_entry(const std::string &key, const std::string &value) {
  // 计算entry大小：key长度(2B) + key + value长度(2B) + value
  size_t entry_size =
      sizeof(uint16_t) + key.size() + sizeof(uint16_t) + value.size();
  size_t old_size = data.size();
  data.resize(old_size + entry_size);

  // 写入key长度
  uint16_t key_len = key.size();
  memcpy(data.data() + old_size, &key_len, sizeof(uint16_t));

  // 写入key
  memcpy(data.data() + old_size + sizeof(uint16_t), key.data(), key_len);

  // 写入value长度
  uint16_t value_len = value.size();
  memcpy(data.data() + old_size + sizeof(uint16_t) + key_len, &value_len,
         sizeof(uint16_t));

  // 写入value
  memcpy(data.data() + old_size + sizeof(uint16_t) + key_len + sizeof(uint16_t),
         value.data(), value_len);

  // 记录偏移
  offsets.push_back(old_size);
}

// 从指定偏移量获取entry的key
std::string Block::get_key_at(size_t offset) const {
  uint16_t key_len;
  memcpy(&key_len, data.data() + offset, sizeof(uint16_t));
  return std::string(
      reinterpret_cast<const char *>(data.data() + offset + sizeof(uint16_t)),
      key_len);
}

// 从指定偏移量获取entry的value
std::string Block::get_value_at(size_t offset) const {
  // 先获取key长度
  uint16_t key_len;
  memcpy(&key_len, data.data() + offset, sizeof(uint16_t));

  // 计算value长度的位置
  size_t value_len_pos = offset + sizeof(uint16_t) + key_len;
  uint16_t value_len;
  memcpy(&value_len, data.data() + value_len_pos, sizeof(uint16_t));

  // 返回value
  return std::string(reinterpret_cast<const char *>(
                         data.data() + value_len_pos + sizeof(uint16_t)),
                     value_len);
}

// 比较指定偏移量处的key与目标key
int Block::compare_key_at(size_t offset, const std::string &target) const {
  std::string key = get_key_at(offset);
  return key.compare(target);
}

// 使用二分查找获取value
// 要求在插入数据时有序插入
std::optional<std::string> Block::get_value_binary(const std::string &key) {
  if (offsets.empty()) {
    return std::nullopt;
  }

  // 二分查找
  int left = 0;
  int right = offsets.size() - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2;
    size_t mid_offset = offsets[mid];

    int cmp = compare_key_at(mid_offset, key);

    if (cmp == 0) {
      // 找到key，返回对应的value
      return get_value_at(mid_offset);
    } else if (cmp < 0) {
      // 中间的key小于目标key，查找右半部分
      left = mid + 1;
    } else {
      // 中间的key大于目标key，查找左半部分
      right = mid - 1;
    }
  }

  return std::nullopt;
}

Block::Entry Block::get_entry_at(size_t offset) const {
  Entry entry;
  entry.key = get_key_at(offset);
  entry.value = get_value_at(offset);
  return entry;
}

size_t Block::size() const { return offsets.size(); }

BlockIterator Block::begin() { return BlockIterator(shared_from_this(), 0); }

BlockIterator Block::end() {
  return BlockIterator(shared_from_this(), offsets.size());
}