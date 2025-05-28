#pragma once

#include "../block/block.h"
#include "../block/block_cache.h"
#include "../block/blockmeta.h"
#include "../utils/bloom_filter.h"
#include "../utils/files.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace toni_lsm {

class vlogIterator;
class FileObj;

/**
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

class vlog : public std::enable_shared_from_this<vlog> {
  friend class vlogBuilder;
  friend std::optional<std::pair<vlogIterator, vlogIterator>>
  vlog_iters_monotony_predicate(
      std::shared_ptr<vlog> vlog, uint64_t tranc_id,
      std::function<int(const std::string &)> predicate);

private:
  struct Entry {
    uint64_t tranc_id; // 事务id
    uint64_t len;
    uint8_t type; // 0: normal, 1: large data
    std::string data;
  };

  // uint64_t file_size = 0;
  size_t vlog_id;

public:
  FileObj file;
  //
  vlog(size_t vlog_id_, FileObj &&file_)
      : vlog_id(vlog_id_), // 成员初始化列表
        file(std::move(file_)){
            // 正确使用移动语义
            // 可选的构造函数逻辑
        };
  std::string get_value_(uint64_t offset);
  uint64_t get_len(uint64_t offset);
  // uint64_t get_tranc_id(uint64_t offset);
  uint8_t get_type(uint64_t offset);
  // 插入value
  void put_(const std::string &value, uint64_t tranc_id, uint64_t &offset);
  Entry get_entry(uint64_t offset);
  uint64_t get_tranc_id_at(uint64_t offset);
  uint64_t get_size() const { return file.size(); } // 获取vlog的大小
  uint64_t get_file_no() const { return vlog_id; }  // 获取vlog的文件编号
};

class vlogBuilder {
private:
  Block block;
  std::string first_key;
  std::string last_key;
  std::vector<BlockMeta> meta_entries;
  std::vector<uint8_t> data;
  size_t block_size;
  std::shared_ptr<BloomFilter> bloom_filter;
  uint64_t min_tranc_id_ = UINT64_MAX;
  uint64_t max_tranc_id_ = 0;

public:
  // 创建一个vlog构建器, 指定目标block的大小
  vlogBuilder(size_t block_size, bool has_bloom); // 添加一个key-value对
  void add(const std::string &key, const std::string &value, uint64_t tranc_id);
  // 估计vlog的大小
  size_t estimated_size() const;
  // 完成当前block的构建, 即将block写入data, 并创建新的block
  void finish_block();
  // 构建vlog, 将vlog写入文件并返回vlog描述类
  std::shared_ptr<vlog> build(size_t vlog_id, const std::string &path,
                              std::shared_ptr<BlockCache> block_cache);
};
} // namespace toni_lsm