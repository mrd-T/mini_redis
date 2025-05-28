

#include "vstt/vlog.h"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>
namespace toni_lsm {

std::string vlog::get_value_(uint64_t offset) { return get_entry(offset).data; }
uint64_t vlog::get_len(uint64_t offset) { return get_entry(offset).len; }
uint64_t vlog::get_tranc_id_at(uint64_t offset) {
  return get_entry(offset).tranc_id;
}
uint8_t vlog::get_type(uint64_t offset) { return get_entry(offset).type; }
void vlog::put_(const std::string &value, uint64_t tranc_id, uint64_t offset) {
  // 准备要写入的数据
  uint64_t vlen = value.size();
  uint8_t type = 0; // 假设类型为0，你可以根据实际情况调整

  // 写入长度字段
  std::vector<u_int8_t> data;

  data.resize(sizeof(uint64_t) * 2 + sizeof(uint8_t) + value.size());
  memcpy(data.data(), &vlen, sizeof(u_int64_t));
  memcpy(data.data() + sizeof(u_int64_t), &type, sizeof(u_int8_t));
  memcpy(data.data() + sizeof(u_int64_t) + sizeof(u_int8_t), &tranc_id,
         sizeof(u_int64_t));
  memcpy(data.data() + 2 * sizeof(u_int64_t) + sizeof(u_int8_t), value.data(),
         vlen);
  file.append(data);
  // spdlog::info("vlog put: offset {}, tranc_id {}, type {}, value {}", offset,
  //              tranc_id, type, value);
  // spdlog::info("Creating filesize: {}", file.size());
}
vlog::Entry vlog::get_entry(uint64_t offset) {
  std::vector<u_int8_t> data;
  size_t file_size = file.size();
  // 读取文件末尾的元数据块
  if (file_size < offset + sizeof(uint64_t) * 2 + sizeof(u_int8_t)) {
    throw std::runtime_error("Invalid SST file: too small");
  }
  auto len = file.read_to_slice(offset, sizeof(uint64_t));
  auto type = file.read_to_slice(offset + sizeof(uint64_t), sizeof(uint8_t));
  auto tranid = file.read_to_slice(offset + sizeof(uint64_t) + sizeof(uint8_t),
                                   sizeof(uint64_t));
  uint64_t vlen;
  uint64_t vtranid;
  uint8_t vtype;
  memcpy(&vlen, len.data(), sizeof(uint64_t));
  memcpy(&vtype, type.data(), sizeof(uint8_t));
  memcpy(&vtranid, tranid.data(), sizeof(uint64_t));
  offset += sizeof(uint64_t) * 2 + sizeof(u_int8_t);
  data = file.read_to_slice(offset, vlen);
  return Entry{vtranid, vlen, vtype,
               std::string(reinterpret_cast<const char *>(data.data()))};
}

// vlog::
} // namespace toni_lsm