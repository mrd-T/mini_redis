-- 定义项目
set_project("toni-lsm")
set_version("0.0.1")
set_languages("c++20")

add_rules("mode.debug", "mode.release")
add_requires("gtest") -- 添加gtest依赖

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
    add_deps("skiplist")
    add_deps("iterator")
    add_files("src/memtable/*.cpp")
    add_includedirs("include", {public = true})

target("block")
    set_kind("static")  -- 生成静态库
    add_files("src/block/*.cpp")
    add_includedirs("include", {public = true})

target("sst")
    set_kind("static")  -- 生成静态库
    add_deps("block")
    add_deps("utils")
    add_files("src/sst/*.cpp")
    add_includedirs("include", {public = true})

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