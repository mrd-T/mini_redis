#include "../../include/utils/files.h"
#include <cstring>
#include <stdexcept>

FileObj::FileObj() : m_file(std::make_unique<MmapFile>()) {}

FileObj::~FileObj() = default;

// 实现移动语义
FileObj::FileObj(FileObj &&other) noexcept
    : m_file(std::move(other.m_file)), m_size(other.m_size) {
  other.m_size = 0;
}

FileObj &FileObj::operator=(FileObj &&other) noexcept {
  if (this != &other) {
    m_file = std::move(other.m_file);
    m_size = other.m_size;
    other.m_size = 0;
  }
  return *this;
}

size_t FileObj::size() const { return m_file->size(); }

void FileObj::set_size(size_t size) { m_size = size; }

FileObj FileObj::create_and_write(const std::string &path,
                                  std::vector<uint8_t> buf) {
  FileObj file_obj;

  // 创建文件，设置大小并映射到内存
  if (!file_obj.m_file->create_and_map(path, buf.size())) {
    throw std::runtime_error("Failed to create and map file: " + path);
  }

  // 写入数据
  memcpy(file_obj.m_file->data(), buf.data(), buf.size());

  // 同步到磁盘
  file_obj.m_file->sync();

  return std::move(file_obj);
}

FileObj FileObj::open(const std::string &path) {
  FileObj file_obj;

  // 打开文件
  if (!file_obj.m_file->open(path)) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  return std::move(file_obj);
}

std::vector<uint8_t> FileObj::read_to_slice(size_t offset, size_t length) {
  // 检查边界
  if (offset + length > m_file->size()) {
    throw std::out_of_range("Read beyond file size");
  }

  // 创建结果vector
  std::vector<uint8_t> result(length);

  // 从映射的内存中复制数据
  const uint8_t *data = static_cast<const uint8_t *>(m_file->data());
  memcpy(result.data(), data + offset, length);

  return result;
}