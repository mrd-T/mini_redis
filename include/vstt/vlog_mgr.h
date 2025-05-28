#pragma once
#include "vstt/vlog.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <sys/types.h>
#include <vector>
namespace toni_lsm {
class vlog;
class vlogManager {
public:
  // 创建一个新的 vlog 管理器
  // vlogManager(size_t block_capacity);

  // 获取一个新的 Vlog
  std::shared_ptr<vlog> get_new_vlog();

  // 将 vlog 添加到管理器中
  void add_vlog(std::shared_ptr<vlog> vlog);

  // 获取当前所有的 vlog
  std::vector<std::shared_ptr<vlog>> get_all_vlogs() const;
  // 获取value
  std::string get_value_(uint64_t vsstid, uint64_t offset);
  // 插入value
  void put_value_(uint64_t vsstid, uint64_t tranc_id, const std::string &value,
                  uint64_t offset);
  // 获取事务id
  uint64_t get_tranc_id_(uint64_t vsstid, uint64_t offset);

  std::atomic<uint64_t> vlogid_ = 1;

private:
  std::vector<std::shared_ptr<vlog>> vlogs_; // 管理vsst
  std::mutex mutex_;
};

} // namespace toni_lsm
