#include "../include/skiplist/skiplist.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

// 测试基本插入、查找和删除
TEST(SkipListTest, BasicOperations) {
    SkipList skipList;

    // 测试插入和查找
    skipList.put("key1", "value1");
    EXPECT_EQ(skipList.get("key1").value(), "value1");

    // 测试更新
    skipList.put("key1", "new_value");
    EXPECT_EQ(skipList.get("key1").value(), "new_value");

    // 测试删除
    skipList.remove("key1");
    EXPECT_FALSE(skipList.get("key1").has_value());
}

// 测试迭代器
TEST(SkipListTest, Iterator) {
    SkipList skipList;
    skipList.put("key1", "value1");
    skipList.put("key2", "value2");
    skipList.put("key3", "value3");

    // 测试迭代器
    std::vector<std::pair<std::string, std::string>> result;
    for (auto it = skipList.begin(); it != skipList.end(); ++it) {
        result.push_back(*it);
    }

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0].first, "key1");
    EXPECT_EQ(result[1].first, "key2");
    EXPECT_EQ(result[2].first, "key3");
}

// 测试大量数据插入和查找
TEST(SkipListTest, LargeScaleInsertAndGet) {
    SkipList skipList;
    const int num_elements = 10000;

    // 插入大量数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        skipList.put(key, value);
    }

    // 验证插入的数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string expected_value = "value" + std::to_string(i);
        EXPECT_EQ(skipList.get(key).value(), expected_value);
    }
}

// 测试大量数据删除
TEST(SkipListTest, LargeScaleRemove) {
    SkipList skipList;
    const int num_elements = 10000;

    // 插入大量数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        skipList.put(key, value);
    }

    // 删除所有数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        skipList.remove(key);
    }

    // 验证所有数据已被删除
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        EXPECT_FALSE(skipList.get(key).has_value());
    }
}

// 测试重复插入
TEST(SkipListTest, DuplicateInsert) {
    SkipList skipList;

    // 重复插入相同的key
    skipList.put("key1", "value1");
    skipList.put("key1", "value2");
    skipList.put("key1", "value3");

    // 验证最后一次插入的值
    EXPECT_EQ(skipList.get("key1").value(), "value3");
}

// 测试空跳表
TEST(SkipListTest, EmptySkipList) {
    SkipList skipList;

    // 验证空跳表的查找和删除
    EXPECT_FALSE(skipList.get("nonexistent_key").has_value());
    skipList.remove("nonexistent_key");  // 删除不存在的key
}

// 测试随机插入和删除
TEST(SkipListTest, RandomInsertAndRemove) {
    SkipList skipList;
    std::unordered_set<std::string> keys;
    const int num_operations = 10000;

    for (int i = 0; i < num_operations; ++i) {
        std::string key = "key" + std::to_string(rand() % 1000);
        std::string value = "value" + std::to_string(rand() % 1000);

        if (keys.find(key) == keys.end()) {
            // 插入新key
            skipList.put(key, value);
            keys.insert(key);
        } else {
            // 删除已存在的key
            skipList.remove(key);
            keys.erase(key);
        }

        // 验证当前状态
        if (keys.find(key) != keys.end()) {
            EXPECT_EQ(skipList.get(key).value(), value);
        } else {
            EXPECT_FALSE(skipList.get(key).has_value());
        }
    }
}

// 测试内存大小跟踪
TEST(SkipListTest, MemorySizeTracking) {
    SkipList skipList;

    // 插入数据
    skipList.put("key1", "value1");
    skipList.put("key2", "value2");

    // 验证内存大小
    size_t expected_size = sizeof("key1") - 1 + sizeof("value1") - 1 +
                           sizeof("key2") - 1 + sizeof("value2") - 1;
    EXPECT_EQ(skipList.get_size(), expected_size);

    // 删除数据
    skipList.remove("key1");
    expected_size -= sizeof("key1") - 1 + sizeof("value1") - 1;
    EXPECT_EQ(skipList.get_size(), expected_size);

    skipList.clear();
    EXPECT_EQ(skipList.get_size(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}