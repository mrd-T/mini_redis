#include "../../include/skiplist/skiplist.h"
#include <stdexcept>

// ************************ SkipListIterator ************************

std::pair<std::string, std::string> SkipListIterator::operator*() const {
  if (!current)
    throw std::runtime_error("Dereferencing invalid iterator");
  return {current->key, current->value};
}

SkipListIterator &SkipListIterator::operator++() {
  if (current) {
    current = current->forward[0];
  }
  return *this;
}

SkipListIterator SkipListIterator::operator++(int) {
  SkipListIterator temp = *this;
  ++(*this);
  return temp;
}

bool SkipListIterator::operator==(const SkipListIterator &other) const {
  return current == other.current;
}

bool SkipListIterator::operator!=(const SkipListIterator &other) const {
  return !(*this == other);
}

std::string SkipListIterator::get_key() const { return current->key; }
std::string SkipListIterator::get_value() const { return current->value; }

bool SkipListIterator::is_valid() const { return !current->value.empty(); }

// ************************ SkipList ************************
// 构造函数
SkipList::SkipList(int max_lvl) : max_level(max_lvl), current_level(1) {
  head = std::make_shared<SkipListNode>("", "", max_level);
}

int SkipList::random_level() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 1);
  int level = 1;
  // 通过"抛硬币"的方式随机生成层数：
  // - 每次有50%的概率增加一层
  // - 确保层数分布为：第1层100%，第2层50%，第3层25%，以此类推
  // - 层数范围限制在[1, max_level]之间，避免浪费内存
  while (dis(gen) && level < max_level) {
    level++;
  }
  return level;
}

// 插入或更新键值对
void SkipList::put(const std::string &key, const std::string &value) {
  std::vector<std::shared_ptr<SkipListNode>> update(max_level, nullptr);

  // std::unique_lock<std::shared_mutex> lock(rw_mutex);
  auto current = head;

  // 从最高层开始查找插入位置
  for (int i = current_level - 1; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->key < key) {
      current = current->forward[i];
    }
    update[i] = current;
  }

  // 移动到最底层
  current = current->forward[0];

  // 如果key已存在，更新value
  if (current && current->key == key) {
    size_bytes += value.size() - current->value.size();
    current->value = value;
    return;
  }

  // 如果key不存在，创建新节点
  int new_level = random_level();
  if (new_level > current_level) {
    for (int i = current_level; i < new_level; ++i) {
      update[i] = head;
    }
    current_level = new_level;
  }

  auto new_node = std::make_shared<SkipListNode>(key, value, new_level);
  size_bytes += key.size() + value.size();

  // 更新各层的指针
  for (int i = 0; i < new_level; ++i) {
    new_node->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = new_node;
  }
}

// 查找键值对
std::optional<std::string> SkipList::get(const std::string &key) {
  // std::shared_lock<std::shared_mutex> slock(rw_mutex);

  auto current = head;
  // 从最高层开始查找
  for (int i = current_level - 1; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->key < key) {
      current = current->forward[i];
    }
  }
  // 移动到最底层
  current = current->forward[0];
  // 如果找到匹配的key，返回value
  if (current && current->key == key) {
    return current->value;
  }
  // 未找到返回空
  return std::nullopt;
}

// 删除键值对
void SkipList::remove(const std::string &key) {
  std::vector<std::shared_ptr<SkipListNode>> update(max_level, nullptr);

  // std::unique_lock<std::shared_mutex> lock(rw_mutex);
  auto current = head;

  // 从最高层开始查找目标节点
  for (int i = current_level - 1; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->key < key) {
      current = current->forward[i];
    }
    update[i] = current;
  }

  // 移动到最底层
  current = current->forward[0];

  // 如果找到目标节点，执行删除操作
  if (current && current->key == key) {
    // 更新每一层的指针，跳过目标节点
    for (int i = 0; i < current_level; ++i) {
      if (update[i]->forward[i] != current) {
        break;
      }
      update[i]->forward[i] = current->forward[i];
    }

    // 更新跳表的内存大小
    size_bytes -= key.size() + current->value.size();

    // 如果删除的节点是最高层的节点，更新跳表的当前层级
    while (current_level > 1 && head->forward[current_level - 1] == nullptr) {
      current_level--;
    }
  }
}

// 刷盘时可以直接遍历最底层链表
std::vector<std::pair<std::string, std::string>> SkipList::flush() {
  // std::shared_lock<std::shared_mutex> slock(rw_mutex);

  std::vector<std::pair<std::string, std::string>> data;
  auto node = head->forward[0];
  while (node) {
    data.emplace_back(node->key, node->value);
    node = node->forward[0];
  }
  return data;
}

size_t SkipList::get_size() {
  // std::shared_lock<std::shared_mutex> slock(rw_mutex);
  return size_bytes;
}

// 清空跳表，释放内存
void SkipList::clear() {
  // std::unique_lock<std::shared_mutex> lock(rw_mutex);
  head = std::make_shared<SkipListNode>("", "", max_level);
  size_bytes = 0;
}

SkipListIterator SkipList::begin() {
  // return SkipListIterator(head->forward[0], rw_mutex);
  return SkipListIterator(head->forward[0]);
}

SkipListIterator SkipList::end() {
  return SkipListIterator(); // 使用空构造函数
}