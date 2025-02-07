#pragma once
#include "../../include/redis_wrapper/redis_wrapper.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

enum class OPS {
  // 基础操作
  GET,
  SET,
  DEL,
  INCR,
  DECR,
  EXPIRE,
  TTL,
  // 哈希操作
  HSET,
  HGET,
  HDEL,
  HKEYS,
  LPUSH,
  RPUSH,
  LPOP,
  RPOP,
  LLEN,
  LRANGE,
  UNKNOWN,
};

OPS string2Ops(const std::string &opStr);

// 基础操作
std::string set_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string get_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string del_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string incr_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string decr_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string expire_handler(std::vector<std::string> &args,
                           RedisWrapper &engine);
std::string ttl_handler(std::vector<std::string> &args, RedisWrapper &engine);

// 哈希操作
std::string hset_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string hget_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string hdel_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string hkeys_handler(std::vector<std::string> &args, RedisWrapper &engine);

// 链表操作
std::string lpush_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string rpush_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string lpop_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string rpop_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string llen_handler(std::vector<std::string> &args, RedisWrapper &engine);
std::string lrange_handler(std::vector<std::string> &args,
                           RedisWrapper &engine);