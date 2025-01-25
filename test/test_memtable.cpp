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

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}