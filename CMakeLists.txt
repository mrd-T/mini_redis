cmake_minimum_required(VERSION 3.15)
project(toni-lsm VERSION 0.0.1 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 配置编译选项
option(LSM_DEBUG "Enable debug mode" OFF)
if(LSM_DEBUG)
    add_compile_definitions(LSM_DEBUG)
endif()
option(ENABLE_DEBUG_FEATURES "Enable debug logging and checks" OFF)

if(ENABLE_DEBUG_FEATURES)
    add_compile_definitions(
        DEBUG_LOG_ENABLED=1
        DEBUG_ASSERT_ENABLED=1
    )
    message(STATUS "Debug features enabled")
endif()
# 查找依赖包
find_package(spdlog REQUIRED)
find_package(toml11 REQUIRED)
find_package(pybind11 REQUIRED)

# 包含目录
include_directories(include)

# 使用 file(GLOB) 收集源文件
file(GLOB LOGGER_SOURCES "src/logger/*.cpp")
file(GLOB CONFIG_SOURCES "src/config/*.cpp")
file(GLOB UTILS_SOURCES "src/utils/*.cpp")
file(GLOB ITERATOR_SOURCES "src/iterator/*.cpp")
file(GLOB SKIPLIST_SOURCES "src/skiplist/*.cpp")
file(GLOB BLOCK_SOURCES "src/block/*.cpp")
file(GLOB SST_SOURCES "src/sst/*.cpp")
file(GLOB MEMTABLE_SOURCES "src/memtable/*.cpp")
file(GLOB WAL_SOURCES "src/wal/*.cpp")
file(GLOB LSM_SOURCES "src/lsm/*.cpp")
file(GLOB REDIS_SOURCES "src/redis_wrapper/*.cpp")
file(GLOB SERVER_SOURCES "server/src/*.cpp")

# 定义所有静态库目标
add_library(logger STATIC ${LOGGER_SOURCES})
target_link_libraries(logger PRIVATE spdlog::spdlog)

add_library(config STATIC ${CONFIG_SOURCES})
target_link_libraries(config PRIVATE toml11::toml11 spdlog::spdlog)

add_library(utils STATIC ${UTILS_SOURCES})
target_link_libraries(utils PRIVATE toml11::toml11 spdlog::spdlog)

add_library(iterator STATIC ${ITERATOR_SOURCES})
target_link_libraries(iterator PRIVATE toml11::toml11 spdlog::spdlog)

add_library(skiplist STATIC ${SKIPLIST_SOURCES})
target_link_libraries(skiplist PRIVATE toml11::toml11 spdlog::spdlog)

add_library(block STATIC ${BLOCK_SOURCES})
target_link_libraries(block PRIVATE config)

add_library(sst STATIC ${SST_SOURCES})
target_link_libraries(sst PRIVATE block utils iterator)

add_library(memtable STATIC ${MEMTABLE_SOURCES})
target_link_libraries(memtable PRIVATE skiplist iterator config sst)

add_library(wal STATIC ${WAL_SOURCES})
target_link_libraries(wal PRIVATE sst memtable)

add_library(lsm STATIC ${LSM_SOURCES})
target_link_libraries(lsm PRIVATE sst memtable wal logger)

add_library(redis STATIC ${REDIS_SOURCES})
target_link_libraries(redis PRIVATE lsm)

# 动态库版本 - 显式列出所有源文件
file(GLOB_RECURSE LSM_SHARED_SOURCES "src/*.cpp")
add_library(lsm_shared SHARED ${LSM_SHARED_SOURCES})
target_link_libraries(lsm_shared PRIVATE toml11::toml11 spdlog::spdlog)
set_target_properties(lsm_shared PROPERTIES OUTPUT_NAME "lsm")
set_target_properties(lsm_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# 安装规则
install(DIRECTORY include/ DESTINATION include/toni-lsm)
install(TARGETS lsm_shared DESTINATION lib)

# 示例程序
add_executable(example example/main.cpp)
target_link_libraries(example PRIVATE lsm_shared fmt::fmt)

add_executable(debug example/debug.cpp)
target_link_libraries(debug PRIVATE lsm_shared)

# 服务器程序 - 设置 muduo 路径
set(muduo_INCLUDE_DIR "/home/mrd/toni-lsm/build/release-install-cpp11/include")
set(muduo_LIBRARY_DIR "/home/mrd/toni-lsm/build/release-install-cpp11/lib")

# 添加 muduo 包含目录和链接目录
include_directories(${muduo_INCLUDE_DIR})
link_directories(${muduo_LIBRARY_DIR})

# 添加服务器可执行文件
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE
    redis
    muduo_net
    muduo_base
    pthread
)

# Python绑定
pybind11_add_module(lsm_pybind sdk/lsm_pybind.cpp)
target_link_libraries(lsm_pybind PRIVATE lsm_shared)
set_target_properties(lsm_pybind PROPERTIES 
    OUTPUT_NAME "lsm_pybind"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
target_compile_definitions(lsm_pybind PRIVATE TONILSM_EXPORT=__attribute__$$visibility$\"default\"$$$)
target_compile_options(lsm_pybind PRIVATE -fvisibility=hidden)