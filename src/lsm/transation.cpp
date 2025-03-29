#include "../../include/lsm/engine.h"
#include "../../include/lsm/transaction.h"
#include "../../include/utils/files.h"
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <vector>

// *********************** TranContext ***********************
TranContext::TranContext(uint64_t tranc_id, std::shared_ptr<LSMEngine> engine,
                         std::shared_ptr<TranManager> tranManager)
    : tranc_id_(tranc_id), engine_(engine), tranManager_(tranManager) {}

void TranContext::put(const std::string &key, const std::string &value) {
  operations.emplace_back(Record::createRecord(this->tranc_id_));
  temp_map_[key] = value;
}

void TranContext::remove(const std::string &key) {
  operations.emplace_back(Record::deleteRecord(this->tranc_id_, key));
  temp_map_[key] = "";
}

std::optional<std::string> TranContext::get(const std::string &key) {
  // 先就近在当前操作的临时缓存中查找
  if (temp_map_.find(key) != temp_map_.end()) {
    return temp_map_[key];
  }
  // 否则使用 engine 查询
  return engine_->get(key, this->tranc_id_);
}

bool TranContext::commit() {
  // commit 需要检查所有的操作是否合法

  // 遍历所有的记录, 判断是否合法

  for (auto &[k, v] : temp_map_) {
    auto check_query = engine_->get(k, tranc_id_);
  }
}

// *********************** TranManager ***********************
TranManager::TranManager(std::string data_dir) : data_dir_(data_dir) {
  auto file_path = get_tranc_id_file_path();

  // 判断文件是否存在

  if (!std::filesystem::exists(file_path)) {
    nextTransactionId_ = 1;
  } else {
    auto tranc_id_file = FileObj::open(file_path, false);
    nextTransactionId_ = tranc_id_file.read_uint64(0);
  }
}

TranManager::~TranManager() {
  auto file_path = get_tranc_id_file_path();
  auto tranc_id_file = FileObj::open(file_path, true);
  std::vector<uint8_t> buf(sizeof(uint64_t), 0);
  uint64_t write_num = nextTransactionId_.load();
  memcpy(buf.data(), &write_num, sizeof(uint64_t));
  tranc_id_file.write(0, buf);
  tranc_id_file.sync();
}

void TranManager::set_engine(std::shared_ptr<LSMEngine> engine) {
  engine_ = engine;
}

uint64_t TranManager::getNextTransactionId() {
  return nextTransactionId_.fetch_add(1, std::memory_order_relaxed);
}

std::shared_ptr<TranContext> TranManager::new_tranc() {
  // 获取锁
  std::unique_lock<std::mutex> lock(mutex_);

  auto tranc_id = getNextTransactionId();
  activeTrans_[tranc_id] =
      std::make_shared<TranContext>(tranc_id, engine_, shared_from_this());
  return activeTrans_[tranc_id];
}
std::string TranManager::get_tranc_id_file_path() {
  if (data_dir_.empty()) {
    data_dir_ = "./";
  }
  return data_dir_ + "/tranc_id";
}