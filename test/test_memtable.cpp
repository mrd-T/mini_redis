#include "../include/memtable/iterator.h"
#include "../include/memtable/memtable.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

// 测试基本的插入和查询操作
TEST(MemTableTest, BasicOperations) {
  MemTable memtable;

  // 测试插入和查找
  memtable.put("key1", "value1");
  EXPECT_EQ(memtable.get("key1").value(), "value1");

  // 测试更新
  memtable.put("key1", "new_value");
  EXPECT_EQ(memtable.get("key1").value(), "new_value");

  // 测试不存在的key
  EXPECT_FALSE(memtable.get("nonexistent").has_value());
}

// 测试删除操作
TEST(MemTableTest, RemoveOperations) {
  MemTable memtable;

  // 插入并删除
  memtable.put("key1", "value1");
  memtable.remove("key1");
  EXPECT_FALSE(memtable.get("key1").has_value());

  // 删除不存在的key
  memtable.remove("nonexistent");
  EXPECT_FALSE(memtable.get("nonexistent").has_value());
}

// 测试冻结表操作
TEST(MemTableTest, FrozenTableOperations) {
  MemTable memtable;

  // 在当前表中插入数据
  memtable.put("key1", "value1");
  memtable.put("key2", "value2");

  // 冻结当前表
  memtable.frozen_cur_table();

  // 在新的当前表中插入数据
  memtable.put("key3", "value3");

  // 验证所有数据都能被访问到
  EXPECT_EQ(memtable.get("key1").value(), "value1");
  EXPECT_EQ(memtable.get("key2").value(), "value2");
  EXPECT_EQ(memtable.get("key3").value(), "value3");
}

// 测试大量数据操作
TEST(MemTableTest, LargeScaleOperations) {
  MemTable memtable;
  const int num_entries = 1000;

  // 插入大量数据
  for (int i = 0; i < num_entries; i++) {
    std::string key = "key" + std::to_string(i);
    std::string value = "value" + std::to_string(i);
    memtable.put(key, value);
  }

  // 验证数据
  for (int i = 0; i < num_entries; i++) {
    std::string key = "key" + std::to_string(i);
    std::string expected = "value" + std::to_string(i);
    EXPECT_EQ(memtable.get(key).value(), expected);
  }
}

// 测试内存大小跟踪
TEST(MemTableTest, MemorySizeTracking) {
  MemTable memtable;

  // 初始大小应该为0
  EXPECT_EQ(memtable.get_total_size(), 0);

  // 添加数据后大小应该增加
  memtable.put("key1", "value1");
  EXPECT_GT(memtable.get_cur_size(), 0);

  // 冻结表后，frozen_size应该增加
  size_t size_before_freeze = memtable.get_total_size();
  memtable.frozen_cur_table();
  EXPECT_EQ(memtable.get_frozen_size(), size_before_freeze);
}

// 测试多次冻结表操作
TEST(MemTableTest, MultipleFrozenTables) {
  MemTable memtable;

  // 第一次冻结
  memtable.put("key1", "value1");
  memtable.frozen_cur_table();

  // 第二次冻结
  memtable.put("key2", "value2");
  memtable.frozen_cur_table();

  // 在当前表中添加数据
  memtable.put("key3", "value3");

  // 验证所有数据都能访问
  EXPECT_EQ(memtable.get("key1").value(), "value1");
  EXPECT_EQ(memtable.get("key2").value(), "value2");
  EXPECT_EQ(memtable.get("key3").value(), "value3");
}

// 测试迭代器在复杂操作序列下的行为
TEST(MemTableTest, IteratorComplexOperations) {
  MemTable memtable;

  // 第一批操作：基本插入
  memtable.put("key1", "value1");
  memtable.put("key2", "value2");
  memtable.put("key3", "value3");

  // 验证第一批操作
  std::vector<std::pair<std::string, std::string>> result1;
  for (auto it = memtable.begin(); it != memtable.end(); ++it) {
    result1.push_back(*it);
  }
  ASSERT_EQ(result1.size(), 3);
  EXPECT_EQ(result1[0].first, "key1");
  EXPECT_EQ(result1[0].second, "value1");
  EXPECT_EQ(result1[2].second, "value3");

  // 冻结当前表
  memtable.frozen_cur_table();

  // 第二批操作：更新和删除
  memtable.put("key2", "value2_updated"); // 更新已存在的key
  memtable.remove("key1");                // 删除一个key
  memtable.put("key4", "value4");         // 插入新key

  // 验证第二批操作
  std::vector<std::pair<std::string, std::string>> result2;
  for (auto it = memtable.begin(); it != memtable.end(); ++it) {
    result2.push_back(*it);
  }
  ASSERT_EQ(result2.size(), 3); // key1被删除，key4被添加
  EXPECT_EQ(result2[0].first, "key2");
  EXPECT_EQ(result2[0].second, "value2_updated");
  EXPECT_EQ(result2[2].first, "key4");

  // 再次冻结当前表
  memtable.frozen_cur_table();

  // 第三批操作：混合操作
  memtable.put("key1", "value1_new"); // 重新插入被删除的key
  memtable.remove("key3"); // 删除一个在第一个frozen table中的key
  memtable.put("key2", "value2_final"); // 再次更新key2
  memtable.put("key5", "value5");       // 插入新key

  // 验证最终结果
  std::vector<std::pair<std::string, std::string>> final_result;
  for (auto it = memtable.begin(); it != memtable.end(); ++it) {
    final_result.push_back(*it);
  }

  // 验证最终状态
  ASSERT_EQ(final_result.size(), 4); // key1, key2, key4, key5

  // 验证具体内容
  EXPECT_EQ(final_result[0].first, "key1");
  EXPECT_EQ(final_result[0].second, "value1_new");

  EXPECT_EQ(final_result[1].first, "key2");
  EXPECT_EQ(final_result[1].second, "value2_final");

  EXPECT_EQ(final_result[2].first, "key4");
  EXPECT_EQ(final_result[2].second, "value4");

  EXPECT_EQ(final_result[3].first, "key5");
  EXPECT_EQ(final_result[3].second, "value5");

  // 验证被删除的key确实不存在
  bool has_key3 = false;
  auto res = memtable.get("key3");
  EXPECT_FALSE(res.has_value());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}