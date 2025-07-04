#include "skiplist/skiplist.h"
#include "sst/sst.h"
#include "vstt/vlog_mgr.h"
#include <cstdint>
#include <memory>
#include <string>
namespace toni_lsm {
constexpr size_t MAX_VALUE_SIZE = 1024 * 1024 * 100; // 1MB, 可根据实际需求调整
class value_manager {
public:
  std::string get_value_from_vlog(std::string key, bool flag,
                                  std::string value) {
    uint64_t FileNo = std::stoull(value.substr(0, 10));
    uint64_t offset = std::stoull(value.substr(10, 20));

    return vlog_mgr_->get_value_(FileNo, offset);
  }
  std::string get_value(std::string key, bool flag, uint64_t file_no,
                        uint64_t offset) {

    return vlog_mgr_->get_value_(file_no, offset);
  }
  //   std::string get_value_from_mem(std::string key, bool flag,
  //                                  std::string value) {
  //     uint64_t FileNo = std::stoull(value.substr(0, 10));
  //     uint64_t offset = std::stoull(value.substr(10, 17));

  //     return vlog_mgr_->get_value_(FileNo, offset);
  //   }
  //   std::string get_value_from_sst(std::string key, bool flag,
  //                                  std::string value) {
  //     uint64_t FileNo = std::stoull(value.substr(0, 10));
  //     uint64_t offset = std::stoull(value.substr(10, 17));

  //     return vlog_mgr_->get_value_(FileNo, offset);
  //   }
  void put_value(std::string key, std::string value, uint64_t tranc_id,
                 uint64_t &file_no, uint64_t &offset) {
    std::shared_ptr<vlog> vlog;
    if (vlog_mgr_->get_size() == 0) {
      vlog = vlog_mgr_->get_new_vlog();
      vlog_mgr_->add_vlog(vlog);
    } else {
      vlog = vlog_mgr_->get_all_vlogs().back();
    }
    if (vlog->get_size() > MAX_VALUE_SIZE) {
      vlog = vlog_mgr_->get_new_vlog();
      vlog_mgr_->add_vlog(vlog);
    }
    vlog->put_(value, tranc_id, offset);
    file_no = vlog->get_file_no();
  }

  value_manager(std::shared_ptr<vlogManager> vsst_mgr)
      : vlog_mgr_(vsst_mgr){

        };

  uint64_t get_tranc_id_(std::string key, bool flag, std::string value) {
    uint64_t FileNo = std::stoull(value.substr(0, 10));
    uint64_t offset = std::stoull(value.substr(10, 17));

    return vlog_mgr_->get_tranc_id_(FileNo, offset);
  }

  std::shared_ptr<vlogManager> vlog_mgr_;

private:
  std::shared_ptr<SkipList> mem_table_;
  std::shared_ptr<SST> sst_;
};
} // namespace toni_lsm