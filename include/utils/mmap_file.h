#pragma once

#include <cstddef>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

class MmapFile {
private:
  int fd_;               // 文件描述符
  void *mapped_data_;    // 映射的内存地址
  size_t file_size_;     // 文件大小
  std::string filename_; // 文件名

public:
  MmapFile() : fd_(-1), mapped_data_(nullptr), file_size_(0) {}
  ~MmapFile() { close(); }

  // 打开文件并映射到内存
  bool open(const std::string &filename, bool create = false);

  // 创建文件并映射到内存
  bool create_and_map(const std::string &path, size_t size);

  // 关闭文件
  void close();

  // 获取映射的内存指针
  void *data() const { return mapped_data_; }

  // 获取文件大小
  size_t size() const { return file_size_; }

  // 写入数据（如果需要）
  bool write(const void *data, size_t size);

  // 同步到磁盘
  bool sync();

private:
  // 禁止拷贝
  MmapFile(const MmapFile &) = delete;
  MmapFile &operator=(const MmapFile &) = delete;
};