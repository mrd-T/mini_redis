#include "../include/block/blockmeta.h"
#include "../include/logger/logger.h"
#include "memtable/memtable.h"
#include "utils/value_mgr.h"
#include "vstt/vlog.h"
#include "vstt/vlog_mgr.h"
#include <gtest/gtest.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <tuple>
#include <vector>

using namespace ::toni_lsm;

class VLOGTEST : public ::testing::Test {

protected:
  // 创建一组测试用的 BlockMeta，确保 key 有序
  std::vector<BlockMeta> createTestMetas() {
    std::vector<BlockMeta> metas;

    // Block 1: offset=0, "a100" -> "a199"
    BlockMeta meta1;
    meta1.offset = 0;
    meta1.first_key = "a100";
    meta1.last_key = "a199";
    metas.push_back(meta1);

    // Block 2: offset=100, "a200" -> "a299"
    BlockMeta meta2;
    meta2.offset = 100;
    meta2.first_key = "a200";
    meta2.last_key = "a299";
    metas.push_back(meta2);

    // Block 3: offset=200, "a300" -> "a399"
    BlockMeta meta3;
    meta3.offset = 200;
    meta3.first_key = "a300";
    meta3.last_key = "a399";
    metas.push_back(meta3);

    return metas;
  }
};
std::string random_string(size_t min_len, size_t max_len) {
  static const std::string charset =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> len_dist(min_len, max_len);
  std::uniform_int_distribution<size_t> char_dist(0, charset.size() - 1);

  size_t length = len_dist(gen);
  std::string result;
  result.reserve(length);

  for (size_t i = 0; i < length; ++i) {
    result += charset[char_dist(gen)];
  }

  return result;
}
TEST_F(VLOGTEST, randomTest) {
  vlogManager::vlogid_ = 0;
  // 重置
  std::vector<std::tuple<int, int, std::string>> f;
  std::shared_ptr<vlogManager> vlog_mgr = std::make_shared<vlogManager>();
  std::shared_ptr<value_manager> value_mgr =
      std::make_shared<value_manager>(vlog_mgr);
  // spdlog::info("vlomanger conut:{}", vlog_mgr.use_count());
  for (int i = 0; i < 10000; i++) {
    std::string key = random_string(1, 1000);
    std::string value = random_string(1, 1000);
    uint64_t tranc_id = i;
    uint64_t offset = 0;
    uint64_t file_no = 0;
    value_mgr->put_value(key, value, tranc_id, file_no, offset);
    f.push_back(std::make_tuple(file_no, offset, value));
    // spdlog::info("file number:{}", file_no);
    // spdlog::info("file size:{}",
    //              value_mgr->vlog_mgr_->get_all_vlogs()[0]->file.m_file->size());
    // spdlog::info("file is offset:{}", offset);
    // EXPECT_EQ(value_mgr->get_value(key, 0, file_no, offset), value);
    // EXPECT_EQ(vlog_mgr->, val2)
  }
  for (int i = 0; i < 1000; i++) {
    int idx = rand() % f.size();
    auto [file_no, offset, value] = f[idx];
    std::string res = value_mgr->get_value("", 0, file_no, offset);
    // spdlog::info("file number:{},offset:{},value:{}", file_no, offset, res);
    EXPECT_EQ(res, value);
  }
}
// // 测试基本的编码和解码功能
// TEST_F(VLOGTEST, orderTest) {
//   // 顺序测试
//   vlogManager::vlogid_ = 0; // 重置 vlogid
//   std::shared_ptr<vlogManager> vlog_mgr = std::make_shared<vlogManager>();
//   std::shared_ptr<value_manager> value_mgr =
//       std::make_shared<value_manager>(vlog_mgr);
//   // spdlog::info("vlomanger conut:{}", vlog_mgr.use_count());
//   for (int i = 0; i < 10000; i++) {
//     std::string key = random_string(1, 1000);
//     std::string value = random_string(1, 1000);
//     uint64_t tranc_id = i;
//     uint64_t offset = 0;
//     uint64_t file_no = 0;
//     value_mgr->put_value(key, value, tranc_id, file_no, offset);
//     // spdlog::info("file number:{}", file_no);
//     // spdlog::info("file size:{}",
//     // value_mgr->vlog_mgr_->get_all_vlogs()[0]->file.m_file->size());
//     // spdlog::info("file is offset:{}", offset);
//     EXPECT_EQ(value_mgr->get_value(key, 0, file_no, offset), value);
//     // EXPECT_EQ(vlog_mgr->, val2)
//   }
// }
// 随机读取

TEST_F(VLOGTEST, memtableTest) {
  // 测试 MemTable 的基本功能

  vlogManager::vlogid_ = 0; // 重置 vlogid
  auto memtable = std::make_shared<MemTable>();
  for (int i = 0; i < 100; i++) {
    // std::string key = "key" + std::to_string(i);
    // std::string value = "value" + std::to_string(i);
    // memtable->put(key, value, 0); // 使用事务ID 0
  }
  // 检查是否可以正确获取值
  for (int i = 0; i < 100; i++) {
    std::string key = "key" + std::to_string(i);
    auto value = memtable->get(key, 0);
    // ASSERT_TRUE(value.get_value());
    ASSERT_EQ(value.get_value(), "value" + std::to_string(i));
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // init_spdlog_file();
  return RUN_ALL_TESTS();
}