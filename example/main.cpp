#include "iostream"
#include "spdlog/spdlog.h"
#include "vstt/vlog_mgr.h"
#include <memory>
using namespace ::toni_lsm;

int main() {
  std::shared_ptr<vlogManager> vlog_mgr = std::make_shared<vlogManager>();
  auto vlog1 = vlog_mgr->get_new_vlog();
  // std::cout << vlog1->file.m_file->file_.is_open();
  // spdlog::info("vlogsize={}", vlog1->get_size());
  vlog_mgr->add_vlog(vlog1);
  // spdlog::info("vlogManager--add_vlog: Added vlog with id={}", vlog1->f);
  vlog1->put_("Hello, Worldsss!sss", 1, 0);
  auto value = vlog1->get_value_(0);
  // std::cout << "Vlog value at offset 0: " << value << std::endl;
  spdlog::info("Vlog value at offset 0: {}", value);
  auto tranc_id = vlog1->get_tranc_id_at(0);
  // std::cout << "Transaction ID at offset 0: " << tranc_id << std::endl;
  spdlog::info("Transaction ID at offset 0: {}", tranc_id);
  auto type = vlog1->get_type(0);
  return 0;
}