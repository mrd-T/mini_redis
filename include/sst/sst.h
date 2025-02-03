#pragma once

#include "../../include/block/block.h"
#include "../../include/block/blockmeta.h"
#include "../../include/utils/files.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class SstIterator;

/**
 * SST文件的结构, 参考自 https://skyzh.github.io/mini-lsm/week1-04-sst.html
 * ------------------------------------------------------------------------
 * |         Block Section         |  Meta Section | Extra                |
 * ------------------------------------------------------------------------
 * | data block | ... | data block |    metadata   | metadata offset (32) |
 * ------------------------------------------------------------------------

 * 其中, metadata 是一个数组加上一些描述信息, 数组每个元素由一个 BlockMeta
 编码形成 MetaEntry, MetaEntry 结构如下:
 * ---------------------------------------------------------------------------------------------------
 * | offset(32) | 1st_key_len(16) | 1st_key(1st_key_len) | last_key_len(16) |
 last_key(last_key_len) |
 * ---------------------------------------------------------------------------------------------------

 * Meta Section 的结构如下:
 * ---------------------------------------------------------------
 * | num_entries (32) | MetaEntry | ... | MetaEntry | Hash (32) |
 * ---------------------------------------------------------------
 * 其中, num_entries 表示 metadata 数组的长度, Hash 是 metadata
 数组的哈希值(只包括数组部分, 不包括 num_entries ), 用于校验 metadata 的完整性
 */

class SST : public std::enable_shared_from_this<SST> {
  friend class SSTBuilder;

private:
  FileObj file;
  std::vector<BlockMeta> meta_entries;
  uint32_t meta_block_offset;
  size_t sst_id;
  std::string first_key;
  std::string last_key;

public:
  // 从文件中打开sst
  static SST open(size_t sst_id, FileObj file);
  // 创建一个sst, 只包含首尾key的元数据
  static SST create_sst_with_meta_only(size_t sst_id, size_t file_size,
                                       const std::string &first_key,
                                       const std::string &last_key);
  // 根据索引读取block
  std::shared_ptr<Block> read_block(size_t block_idx);

  // 找到key所在的block的idx
  size_t find_block_idx(const std::string &key);

  // 根据key返回迭代器
  SstIterator get(const std::string &key);

  // 返回sst中block的数量
  size_t num_blocks() const;

  // 返回sst的首key
  std::string get_first_key() const;

  // 返回sst的尾key
  std::string get_last_key() const;

  // 返回sst的大小
  size_t sst_size() const;

  // 返回sst的id
  size_t get_sst_id() const;

  SstIterator begin();
  SstIterator end();
};

class SSTBuilder {
private:
  Block block;
  std::string first_key;
  std::string last_key;
  std::vector<BlockMeta> meta_entries;
  std::vector<uint8_t> data;
  size_t block_size;
  std::vector<uint32_t> key_hashes;

public:
  // 创建一个sst构建器, 指定目标block的大小
  SSTBuilder(size_t block_size);
  // 添加一个key-value对
  void add(const std::string &key, const std::string &value);
  // 估计sst的大小
  size_t estimated_size() const;
  // 完成当前block的构建, 即将block写入data, 并创建新的block
  void finish_block();
  // 构建sst, 将sst写入文件并返回SST描述类
  SST build(size_t sst_id, const std::string &path);
};
