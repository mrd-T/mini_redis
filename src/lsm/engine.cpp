#include "../../include/lsm/engine.h"
#include "../../include/consts.h"
#include "../../include/sst/sst.h"
#include "../../include/sst/sst_iterator.h"
#include <filesystem>
#include <vector>

// *********************** LSMEngine ***********************
LSMEngine::LSMEngine(std::string path) : data_dir(path) {
  // 创建数据目录
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
  } else {
    // 如果目录存在，则检查是否有 sst 文件并加载
    // TODO
  }
}

std::optional<std::string> LSMEngine::get(const std::string &key) {
  // 1. 先查找 memtable
  auto value = memtable.get(key);
  if (value.has_value()) {
    if (value.value().size() > 0) {
      // 值存在且不为空（没有被删除）
      return value;
    } else {
      // 空值表示被删除了
      return std::nullopt;
    }
  }

  // 2. l0 sst中查询
  for (auto &sst_id : l0_sst_ids) {
    auto sst = ssts[sst_id];
    auto sst_iterator = sst->get(key);
    if (sst_iterator != sst->end()) {
      if ((*sst_iterator).second.size() > 0) {
        // 值存在且不为空（没有被删除）
        return (*sst_iterator).second;
      } else {
        // 空值表示被删除了
        return std::nullopt;
      }
    }
  }

  return std::nullopt;
}

void LSMEngine::put(const std::string &key, const std::string &value) {
  memtable.put(key, value);

  // 如果 memtable 太大，需要刷新到磁盘
  if (memtable.get_total_size() >= TOL_MEM_SIZE_LIMIT) {
    flush();
  }
}

void LSMEngine::remove(const std::string &key) {
  // 在 LSM 中，删除实际上是插入一个空值
  memtable.remove(key);
}

void LSMEngine::flush() {
  if (memtable.get_total_size() == 0) {
    return;
  }

  // 1. 创建新的 SST ID
  size_t new_sst_id = l0_sst_ids.empty() ? 0 : l0_sst_ids.back() + 1;

  // 2. 构建 SST
  SSTBuilder builder(4096); // 4KB block size

  // 3. 将 memtable 中的数据写入 SST
  auto mem_iter = memtable.begin();
  while (!mem_iter.is_end()) {
    auto [key, value] = *mem_iter;
    builder.add(key, value);
    ++mem_iter;
  }

  // 4. 写入文件
  auto sst =
      builder.build(new_sst_id, "test_data/sst_" + std::to_string(new_sst_id));

  // 5. 更新 sst_ids
  l0_sst_ids.push_back(new_sst_id);

  // 6. 清空 memtable
  memtable.clear();
}

std::string LSMEngine::get_sst_path(size_t sst_id) {
  return data_dir + "/sst_" + std::to_string(sst_id);
}

MergeIterator LSMEngine::begin() {
  std::vector<SearchItem> item_vec;
  for (auto &sst_id : l0_sst_ids) {
    auto sst = ssts[sst_id];
    for (auto iter = sst->begin(); iter != sst->end(); ++iter) {
      item_vec.emplace_back(iter.key(), iter.value(), sst_id);
    }
  }
  HeapIterator l0_iter(item_vec);

  auto mem_iter = memtable.begin();

  return MergeIterator(mem_iter, l0_iter);
}

MergeIterator LSMEngine::end() { return MergeIterator{}; }

// *********************** LSM ***********************
LSM::LSM(std::string path) : engine(path) {}

LSM::~LSM() {
  // 确保所有数据都已经刷新到磁盘
  engine.flush();
}

std::optional<std::string> LSM::get(const std::string &key) {
  return engine.get(key);
}
void LSM::put(const std::string &key, const std::string &value) {
  engine.put(key, value);
}
void LSM::remove(const std::string &key) { engine.remove(key); }

LSM::LSMIterator LSM::begin() { return engine.begin(); }
LSM::LSMIterator LSM::end() { return engine.end(); }