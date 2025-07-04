
#include "vstt/vlog_mgr.h"
#include "vstt/vlog.h"
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <vector>
namespace toni_lsm {

// 获取一个新的 Vlog
std::shared_ptr<vlog> vlogManager::get_new_vlog() {

  FileObj file = FileObj::create_and_write(
      "test_lsm_data/vlog_" + std::to_string(vlogid_) + ".vlog",
      std::vector<uint8_t>());
  // std::cout << file.m_file->file_.is_open();
  // spdlog::info("是否打开: {}", file.m_file->file_.is_open());
  auto p = std::make_shared<vlog>(vlogid_, std::move(file));
  vlogid_.fetch_add(1);
  return p;
}
void vlogManager::open_vlog(const std ::string &path, uint64_t vlog_it) {
  FileObj file = FileObj::create_and_write(path, std::vector<uint8_t>());
  // std::cout << file.m_file->file_.is_open();
  // spdlog::info("是否打开: {}", file.m_file->file_.is_open());
  auto p = std::make_shared<vlog>(vlogid_, std::move(file));
  add_vlog(p);
}
// 将 vlog 添加到管理器中
void vlogManager::add_vlog(std::shared_ptr<vlog> vlog) {
  std::lock_guard<std::mutex> lock(mutex_);
  vlogs_.push_back(vlog);
  // spdlog::info("vlogManager--add_vlog: Added vlog with id={}",
  // vlog->vlog_id); spdlog::info("vlogsize={}", vlog->get_size());
}

// 获取当前所有的 vlog
std::vector<std::shared_ptr<vlog>> vlogManager::get_all_vlogs() const {
  return vlogs_;
}
// 获取value
std::string vlogManager::get_value_(uint64_t vsstid, uint64_t offset) {
  return vlogs_[vsstid]->get_value_(offset);
}
// 插入value
void vlogManager::put_value_(uint64_t vsstid, uint64_t tranc_id,
                             const std::string &value, uint64_t offset) {
  vlogs_[vsstid]->put_(value, tranc_id, offset);
}
// 获取事务id
uint64_t vlogManager::get_tranc_id_(uint64_t vsstid, uint64_t offset) {
  return vlogs_[vsstid]->get_tranc_id_at(offset);
}

} // namespace toni_lsm
