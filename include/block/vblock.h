#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

/***
Refer to https://skyzh.github.io/mini-lsm/week1-03-block.html for memory layout

--------------------------------------
|             Data Section           |
--------------------------------------
|Entry#1|Entry#2|...|Entry#N|
--------------------------------------

---------------------------------------------------------------------
|                           Entry #1 |                          ... |
--------------------------------------------------------------|-----|
|len(64B)|type(8B)  |tranc_id(64B)|data(data_len)|
---------------------------------------------------------------------

*/

namespace toni_lsm {
class BlockIterator;

class VBlock : public std::enable_shared_from_this<VBlock> {
  friend BlockIterator;

private:
  std::vector<uint8_t> data;
  size_t capacity;

  struct Entry {
    uint64_t tranc_id; // 事务id
    uint64_t len;
    uint8_t type; // 0: normal, 1: large data
    std::string data;
  };

  std::string get_value_at(size_t offset) const;
  uint16_t get_tranc_id_at(size_t offset) const;
  // 根据id的可见性调整位置
  int adjust_idx_by_tranc_id(size_t idx, uint64_t tranc_id);
  Entry get_Entry_at(size_t offset) const;

public:
  VBlock() = default;
  VBlock(size_t capacity);
  // ! 这里的编码函数不包括 hash
  std::vector<uint8_t> encode();
  // ! 这里的解码函数可指定切片是否包括 hash
  static std::shared_ptr<VBlock> decode(const std::vector<uint8_t> &encoded,
                                        bool with_hash = false);
  std::string get_first_key();
  size_t get_offset_at(size_t idx) const;
  bool add_entry(const std::string &key, const std::string &value,
                 uint64_t tranc_id, bool force_write);
  std::optional<std::string> get_value_binary(const std::string &key,
                                              uint64_t tranc_id);

  size_t size() const;
  size_t cur_size() const;
  bool is_empty() const;
  std::optional<size_t> get_idx_binary(const std::string &key,
                                       uint64_t tranc_id);
  BlockIterator end();
};
} // namespace toni_lsm
