#pragma once

#include <memory>
#include <string>

class BaseIterator {
public:
  virtual std::pair<std::string, std::string> operator*() const {
    return std::make_pair("", "");
  }
  virtual bool operator==(const BaseIterator &other) const { return false; }
  virtual bool operator!=(const BaseIterator &other) const { return false; }
  virtual bool is_end() const { return false; }
};

struct SearchItem {
  std::string key;
  std::string value;
  int idx;
};

bool operator<(const SearchItem &a, const SearchItem &b);
bool operator>(const SearchItem &a, const SearchItem &b);
bool operator==(const SearchItem &a, const SearchItem &b);
