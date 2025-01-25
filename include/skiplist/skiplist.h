#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstddef>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

struct SkipListNode {
  std::string key;   // 节点存储的键
  std::string value; // 节点存储的值
  std::vector<std::shared_ptr<SkipListNode>>
      forward; // 指向不同层级的下一个节点的指针数组

  SkipListNode(const std::string &k, const std::string &v, int level)
      : key(k), value(v), forward(level, nullptr) {}
};

class SkipListIterator {
public:
  SkipListIterator(std::shared_ptr<SkipListNode> node) : current(node) {}

  std::pair<std::string, std::string> operator*() const;

  SkipListIterator &operator++(); // 前置自增

  SkipListIterator operator++(int); // 后置自增

  bool operator==(const SkipListIterator &other) const;

  bool operator!=(const SkipListIterator &other) const;

  std::string get_key() const;
  std::string get_value() const;

  bool is_valid() const;

private:
  std::shared_ptr<SkipListNode> current;
};

class SkipList {
private:
  std::shared_ptr<SkipListNode>
      head; // 跳表的头节点，不存储实际数据，用于遍历跳表
  int max_level;     // 跳表的最大层级数，限制跳表的高度
  int current_level; // 跳表当前的实际层级数，动态变化
  size_t size_bytes = 0; // 跳表当前占用的内存大小（字节数），用于跟踪内存使用

  int random_level(); // 生成新节点的随机层级数

public:
  SkipList(int max_lvl = 16); // 构造函数，初始化跳表

  void put(const std::string &key,
           const std::string &value); // 插入或更新键值对

  std::optional<std::string>
  get(const std::string &key) const; // 查找键对应的值

  // !!! 这里的 remove 是跳表本身真实的 remove,  lsm 应该使用 put 空值表示删除
  void remove(const std::string &key); // 删除键值对

  std::vector<std::pair<std::string, std::string>>
  flush(); // 将跳表数据刷出，返回有序键值对列表

  size_t get_size() const;

  void clear(); // 清空跳表，释放内存

  SkipListIterator begin() const { return SkipListIterator(head->forward[0]); }

  SkipListIterator end() const { return SkipListIterator(nullptr); }
};

#endif // SKIPLIST_H
