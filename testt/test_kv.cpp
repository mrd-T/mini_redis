// #include "../include/config/config.h"
// #include "../include/logger/logger.h"
// #include "memtable/memtable.h"
// #include <gtest/gtest.h>
// #include <memory>

// using namespace ::toni_lsm;

// TEST(ConfigTest, BasicTest) {
//   // 编译后的单元测试位置为: build/linux/x86_64/debug/test_config
//   // 而配置文件放在项目根目录
//   auto memtable = std::make_shared<MemTable>();
//   for (int i = 0; i < 100; i++) {
//     std::string key = "key" + std::to_string(i);
//     std::string value = "value" + std::to_string(i);
//     memtable->put(key, value, 0); // 使用事务ID 0
//   }
//   // 检查是否可以正确获取值
//   for (int i = 0; i < 100; i++) {
//     std::string key = "key" + std::to_string(i);
//     auto value = memtable->get(key, 0);
//     // ASSERT_TRUE(value.get_value());
//     ASSERT_EQ(value.get_value(), "value" + std::to_string(i));
//   }
// }

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   init_spdlog_file();
//   return RUN_ALL_TESTS();
// }