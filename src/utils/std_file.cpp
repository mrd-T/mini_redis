#include "../../include/utils/std_file.h"

bool StdFile::open(const std::filesystem::path &filename, Mode mode) {
  filename_ = filename;
  mode_ = mode;

  switch (mode) {
  case Mode::READ:
    return open_standard(filename, std::ios::in | std::ios::binary);
  case Mode::WRITE:
    return open_standard(filename, std::ios::out | std::ios::binary);
  case Mode::READ_WRITE:
    return open_standard(filename,
                         std::ios::in | std::ios::out | std::ios::binary);
  default:
    return false;
  }
}

void StdFile::close() {
  if (file_.is_open()) {
    file_.close();
  }
}

size_t StdFile::read(void *buffer, size_t size) {
  if (!file_.is_open() || mode_ == Mode::WRITE) {
    return 0;
  }
  file_.read(static_cast<char *>(buffer), size);
  return file_.gcount();
}

size_t StdFile::write(const void *buffer, size_t size) {
  if (!file_.is_open() || mode_ == Mode::READ) {
    return 0;
  }
  file_.write(static_cast<const char *>(buffer), size);
  return size;
}

void StdFile::seekg(std::streampos pos, std::ios::seekdir dir) {
  if (file_.is_open()) {
    file_.seekg(pos, dir);
  }
}

bool StdFile::sync() {
  if (!file_.is_open()) {
    return false;
  }
  file_.flush();
  return file_.good();
}

bool StdFile::open_standard(const std::filesystem::path &filename,
                            std::ios::openmode mode) {
  file_.open(filename, mode);
  return file_.is_open();
}