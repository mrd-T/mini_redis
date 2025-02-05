#pragma once

#include <memory>
#include <queue>
#include <string>

// *************************** SearchItem ***************************
struct SearchItem {
  std::string key;
  std::string value;
  int idx;

  SearchItem() = default;
  SearchItem(std::string k, std::string v, int i)
      : key(std::move(k)), value(std::move(v)), idx(i) {}
};

bool operator<(const SearchItem &a, const SearchItem &b);
bool operator>(const SearchItem &a, const SearchItem &b);
bool operator==(const SearchItem &a, const SearchItem &b);

// *************************** HeapIterator ***************************
class HeapIterator {
  using value_type = std::pair<std::string, std::string>;
  using pointer = value_type *;
  using reference = value_type &;

public:
  HeapIterator() = default;
  HeapIterator(std::vector<SearchItem> item_vec);
  virtual pointer operator->() const;
  virtual value_type operator*() const;
  HeapIterator &operator++();
  HeapIterator operator++(int) = delete;
  virtual bool operator==(const HeapIterator &other) const;
  virtual bool operator!=(const HeapIterator &other) const;
  virtual bool is_end() const;

private:
  std::priority_queue<SearchItem, std::vector<SearchItem>,
                      std::greater<SearchItem>>
      items;
  mutable std::shared_ptr<value_type> current; // 用于存储当前元素

  void update_current() const;
};
