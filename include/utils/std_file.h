#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>

class StdFile {
public:
  enum class Mode { READ, WRITE, READ_WRITE };

private:
  std::fstream file_;
  std::filesystem::path filename_;
  Mode mode_;

public:
  StdFile() : mode_(Mode::READ) {}
  ~StdFile() {
    if (file_.is_open()) {
      close();
    }
  }

  bool open(const std::filesystem::path &filename, Mode mode);
  void close();

  size_t read(void *buffer, size_t size);
  size_t write(const void *buffer, size_t size);
  void seekg(std::streampos pos, std::ios::seekdir dir);

  bool sync();

private:
  bool open_standard(const std::filesystem::path &filename,
                     std::ios::openmode mode);
};