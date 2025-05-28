cmake_minimum_required(VERSION 3.14)
project(AddTest LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
# 查找GoogleTest
find_package(GTest REQUIRED)

# 添加可执行文件
add_executable(add_test
    mrd.cpp  # 假设你的代码保存在这个文件中
)

# 链接GoogleTest库
target_link_libraries(add_test
    PRIVATE
    GTest::GTest
    GTest::Main
)

# 添加测试
enable_testing()
add_test(NAME AddTest COMMAND add_test)