#pragma once

#include <memory>
#include <queue>
#include <string>

// *************************** SearchItem ***************************
struct SearchItem {
  std::string key;
  std::string value;
  int idx;
};

bool operator<(const SearchItem &a, const SearchItem &b);
bool operator>(const SearchItem &a, const SearchItem &b);
bool operator==(const SearchItem &a, const SearchItem &b);

// *************************** HeapIterator ***************************
class HeapIterator {
public:
  HeapIterator() = default;
  HeapIterator(std::vector<SearchItem> item_vec);
  virtual std::pair<std::string, std::string> operator*() const;
  HeapIterator &operator++();
  HeapIterator operator++(int) = delete;
  virtual bool operator==(const HeapIterator &other) const;
  virtual bool operator!=(const HeapIterator &other) const;
  virtual bool is_end() const;

private:
  std::priority_queue<SearchItem, std::vector<SearchItem>,
                      std::greater<SearchItem>>
      items;
};
