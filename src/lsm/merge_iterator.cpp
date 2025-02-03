#include "../../include/lsm/merge_iterator.h"

MergeIterator::MergeIterator() {}

MergeIterator::MergeIterator(HeapIterator it_a, HeapIterator it_b)
    : it_a(std::move(it_a)), it_b(std::move(it_b)) {
  skip_it_b();              // 跳过与 it_a 重复的 key
  choose_a = choose_it_a(); // 决定使用哪个迭代器
}

bool MergeIterator::choose_it_a() {
  if (it_a.is_end()) {
    return false;
  }
  if (it_b.is_end()) {
    return true;
  }
  return (*it_a).first < (*it_b).first; // 比较 key
}

void MergeIterator::skip_it_b() {
  if (!it_a.is_end() && !it_b.is_end() && (*it_b).first == (*it_a).first) {
    ++it_b;
  }
}

bool MergeIterator::is_end() const { return it_a.is_end() && it_b.is_end(); }

std::pair<std::string, std::string> MergeIterator::operator*() const {
  if (choose_a) {
    return *it_a;
  } else {
    return *it_b;
  }
}

MergeIterator &MergeIterator::operator++() {
  if (choose_a) {
    ++it_a;
  } else {
    ++it_b;
  }
  skip_it_b();              // 跳过重复的 key
  choose_a = choose_it_a(); // 重新决定使用哪个迭代器
  return *this;
}

bool MergeIterator::operator==(const MergeIterator &other) const {
  return it_a == other.it_a && it_b == other.it_b && choose_a == other.choose_a;
}

bool MergeIterator::operator!=(const MergeIterator &other) const {
  return !(*this == other);
}