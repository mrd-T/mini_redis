-- 定义项目
set_project("toni-lsm")
set_version("0.0.1")

add_rules("mode.debug", "mode.release")
add_requires("gtest")

target("skiplist")
    set_kind("static")  -- 生成静态库
    add_files("src/skiplist/skipList.cpp")
    add_includedirs("include", {public = true})

target("memtable")
    set_kind("static")  -- 生成静态库
    add_deps("skiplist")
    add_files("src/memtable/memtable.cpp")
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