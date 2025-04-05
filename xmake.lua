-- 定义项目
set_project("toni-lsm")
set_version("0.0.1")
set_languages("c++20")

add_rules("mode.debug", "mode.release")
add_requires("gtest") -- 添加gtest依赖
-- 添加Muduo库
add_requires("muduo")

if is_mode("debug") then
    add_defines("LSM_DEBUG")
end


target("utils")
    set_kind("static")  -- 生成静态库
    add_files("src/utils/*.cpp")
    add_includedirs("include", {public = true})

target("iterator")
    set_kind("static")  -- 生成静态库
    add_files("src/iterator/*.cpp")
    add_includedirs("include", {public = true})

target("skiplist")
    set_kind("static")  -- 生成静态库
    add_files("src/skiplist/*.cpp")
    add_includedirs("include", {public = true})

target("memtable")
    set_kind("static")  -- 生成静态库
    add_deps("skiplist","iterator")
    add_deps("sst")
    add_files("src/memtable/*.cpp")
    add_includedirs("include", {public = true})

target("block")
    set_kind("static")  -- 生成静态库
    add_files("src/block/*.cpp")
    add_includedirs("include", {public = true})

target("sst")
    set_kind("static")  -- 生成静态库
    add_deps("block", "utils", "iterator")
    add_files("src/sst/*.cpp")
    add_includedirs("include", {public = true})

target("wal")
    set_kind("static")  -- 生成静态库
    add_deps("sst", "memtable")
    add_files("src/wal/*.cpp")
    add_includedirs("include", {public = true})

target("lsm")
    set_kind("static")  -- 生成静态库
    add_deps("sst", "memtable", "wal")
    add_files("src/lsm/*.cpp")
    add_includedirs("include", {public = true})

target("redis")
    set_kind("static")  -- 生成静态库
    add_deps("lsm")
    add_files("src/redis_wrapper/*.cpp")
    add_includedirs("include", {public = true})

-- 定义动态链接库目标
target("lsm_shared")
    set_kind("shared")
    add_files("src/**.cpp")
    add_includedirs("include", {public = true})
    set_targetdir("$(buildir)/lib")

    -- 安装头文件和动态链接库
    on_install(function (target)
        os.cp("include", path.join(target:installdir(), "include/toni-lsm"))
        os.cp(target:targetfile(), path.join(target:installdir(), "lib"))
    end)

-- 定义测试
target("test_skiplist")
    set_kind("binary")  -- 生成可执行文件
    set_group("tests")
    add_files("test/test_skiplist.cpp")
    add_deps("skiplist")  -- 依赖skiplist库
    add_packages("gtest")  -- 添加gtest包

target("test_memtable")
    set_kind("binary")
    add_files("test/test_memtable.cpp")
    add_deps("memtable")  -- 如果memtable是独立的target，这里需要添加对应的依赖
    add_packages("gtest")
    add_includedirs("include")

target("test_block")
    set_kind("binary")
    add_files("test/test_block.cpp")
    add_deps("block")  -- 如果memtable是独立的target，这里需要添加对应的依赖
    add_packages("gtest")
    add_includedirs("include")

target("test_blockmeta")
    set_kind("binary")
    add_files("test/test_blockmeta.cpp")
    add_deps("block")  -- 如果memtable是独立的target，这里需要添加对应的依赖
    add_packages("gtest")
    add_includedirs("include")

target("test_utils")
    set_kind("binary")
    add_files("test/test_utils.cpp")
    add_deps("utils")
    add_packages("gtest")
    add_includedirs("include")

target("test_sst")
    set_kind("binary")
    add_files("test/test_sst.cpp")
    add_deps("sst")
    add_packages("gtest")
    add_includedirs("include")

target("test_lsm")
    set_kind("binary")
    add_files("test/test_lsm.cpp")
    add_deps("lsm", "memtable", "iterator")  -- Added memtable and iterator dependencies
    add_packages("gtest")
    add_includedirs("include")

target("test_block_cache")
    set_kind("binary")
    add_files("test/test_block_cache.cpp")
    add_deps("block")
    add_includedirs("include")
    add_packages("gtest")

target("test_compact")
    set_kind("binary")
    add_files("test/test_compact.cpp")
    add_deps("lsm", "memtable", "iterator")  -- Added memtable and iterator dependencies
    add_packages("gtest")
    add_includedirs("include")

target("test_redis")
    set_kind("binary")
    add_files("test/test_redis.cpp")
    add_deps("redis", "memtable", "iterator")  -- Added memtable and iterator dependencies
    add_includedirs("include")
    add_packages("gtest")

target("test_wal")
    set_kind("binary")
    add_files("test/test_wal.cpp")
    add_deps("wal")  -- Added memtable and iterator dependencies
    add_includedirs("include")
    add_packages("gtest")

-- 定义 示例
target("example")
    set_kind("binary")
    add_files("example/main.cpp")
    add_deps("lsm_shared")
    add_includedirs("include", {public = true})
    set_targetdir("$(buildir)/bin")

-- 定义 debug 示例
target("debug")
    set_kind("binary")
    add_files("example/debug.cpp")
    add_deps("lsm_shared")
    add_includedirs("include", {public = true})
    set_targetdir("$(buildir)/bin")

-- 定义server
target("server")
    set_kind("binary")
    add_files("server/src/*.cpp")
    add_deps("redis")
    add_includedirs("include", {public = true})
    add_packages("muduo")
    set_targetdir("$(buildir)/bin")