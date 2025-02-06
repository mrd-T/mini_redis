#include "../../include/block/block_cache.h"
#include "../../include/block/block.h"
#include <chrono>

BlockCache::BlockCache(size_t capacity, size_t k)
    : capacity_(capacity), k_(k) {}

BlockCache::~BlockCache() = default;

std::shared_ptr<Block> BlockCache::get(int sst_id, int block_id) {
  std::lock_guard<std::mutex> lock(mutex_);
  ++total_requests_; // 增加总请求数
  auto key = std::make_pair(sst_id, block_id);
  auto it = cache_map_.find(key);
  if (it == cache_map_.end()) {
    return nullptr; // 缓存未命中
  }

  ++hit_requests_; // 增加命中请求数
  // 更新访问时间
  update_access_time(it->second);

  return it->second->cache_block;
}

void BlockCache::put(int sst_id, int block_id, std::shared_ptr<Block> block) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto key = std::make_pair(sst_id, block_id);
  auto it = cache_map_.find(key);

  if (it != cache_map_.end()) {
    // 更新已有缓存项
    // ! 照理说 Block 类的数据是不可变的，这里的更新分支应该不会存在,
    // 只是debug用
    it->second->cache_block = block;
    update_access_time(it->second);
  } else {
    // 插入新缓存项
    if (cache_list_.size() >= capacity_) {
      // 移除最久未使用的缓存项
      cache_map_.erase(std::make_pair(cache_list_.back().sst_id,
                                      cache_list_.back().block_id));
      cache_list_.pop_back();
    }

    CacheItem item = {sst_id, block_id, block, {current_time()}};
    cache_list_.push_front(item);
    cache_map_[key] = cache_list_.begin();
  }
}

double BlockCache::hit_rate() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return total_requests_ == 0
             ? 0.0
             : static_cast<double>(hit_requests_) / total_requests_;
}

size_t BlockCache::current_time() const {
  return std::chrono::steady_clock::now().time_since_epoch().count();
}

void BlockCache::update_access_time(std::list<CacheItem>::iterator it) {
  it->access_times.push_back(current_time());
  if (it->access_times.size() > k_) {
    it->access_times.erase(it->access_times.begin());
  }

  // 将缓存项移动到链表头部
  cache_list_.splice(cache_list_.begin(), cache_list_, it);
}