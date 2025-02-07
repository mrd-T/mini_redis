#include "../include/handler.h"
#include <muduo/base/Logging.h>

static bool G_DEBUG = true;

// 将字符串转换为小写
std::string toLower(const std::string &str) {
  std::string lowerStr = str;
  std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lowerStr;
}

// 将操作类型字符串转换为枚举类型 OPS
OPS string2Ops(const std::string &opStr) {
  std::string lowerOpStr = toLower(opStr);

  if (lowerOpStr == "get") {
    return OPS::GET;
  } else if (lowerOpStr == "set") {
    return OPS::SET;
  } else if (lowerOpStr == "del") {
    return OPS::DEL;
  } else if (lowerOpStr == "incr") {
    return OPS::INCR;
  } else if (lowerOpStr == "decr") {
    return OPS::DECR;
  } else if (lowerOpStr == "expire") {
    return OPS::EXPIRE;
  } else if (lowerOpStr == "ttl") {
    return OPS::TTL;
  } else if (lowerOpStr == "hset") {
    return OPS::HSET;
  } else if (lowerOpStr == "hget") {
    return OPS::HGET;
  } else if (lowerOpStr == "hdel") {
    return OPS::HDEL;
  } else if (lowerOpStr == "hkeys") {
    return OPS::HKEYS;
  } else if (lowerOpStr == "rpush") {
    return OPS::RPUSH;
  } else if (lowerOpStr == "lpush") {
    return OPS::LPUSH;
  } else if (lowerOpStr == "rpop") {
    return OPS::RPOP;
  } else if (lowerOpStr == "lpop") {
    return OPS::LPOP;
  } else if (lowerOpStr == "llen") {
    return OPS::LLEN;
  } else if (lowerOpStr == "lrange") {
    return OPS::LRANGE;
  } else {
    return OPS::UNKNOWN;
  }
}

// **************************** 基础操作 ****************************
std::string set_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 3) {
    return "-ERR wrong number of arguments for 'SET' command\r\n";
  }

  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << '\n';
  }

  return engine.set(args);
}

std::string get_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 2)
    return "-ERR wrong number of arguments for 'GET' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }

  return engine.get(args);
}

std::string del_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() < 2)
    return "-ERR wrong number of arguments for 'DEL' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }

  return engine.del(args);
}

std::string incr_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 2)
    return "-ERR wrong number of arguments for 'INCR' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }

  auto res = engine.incr(args);

  return ":" + res + "\r\n";
}

std::string decr_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 2)
    return "-ERR wrong number of arguments for 'DECR' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }

  auto res = engine.decr(args);

  return ":" + res + "\r\n";
}

std::string expire_handler(std::vector<std::string> &args,
                           RedisWrapper &engine) {
  if (args.size() != 3)
    return "-ERR wrong number of arguments for 'EXPIRE' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << '\n';
  }

  return engine.expire(args);
}

std::string ttl_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 2)
    return "-ERR wrong number of arguments for 'TTL' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }

  return engine.ttl(args);
}
// **************************** 哈希操作 ****************************
std::string hset_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 4)
    return "-ERR wrong number of arguments for 'HSET' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << " " << args[3] << '\n';
  }

  return engine.hset(args);
}

std::string hget_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 3)
    return "-ERR wrong number of arguments for 'HGET' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << '\n';
  }

  return engine.hget(args);
}

std::string hdel_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 3)
    return "-ERR wrong number of arguments for 'HDEL' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << '\n';
  }

  return engine.hdel(args);
}

std::string hkeys_handler(std::vector<std::string> &args,
                          RedisWrapper &engine) {
  if (args.size() != 2)
    return "-ERR wrong number of arguments for 'HKEYS' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }

  return engine.hkeys(args);
}

// ***************************** 链表操作 ****************************
std::string lpush_handler(std::vector<std::string> &args,
                          RedisWrapper &engine) {
  if (args.size() != 3)
    return "-ERR wrong number of arguments for 'LPUSH' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << '\n';
  }
  return engine.lpush(args);
}

std::string rpush_handler(std::vector<std::string> &args,
                          RedisWrapper &engine) {
  if (args.size() != 3)
    return "-ERR wrong number of arguments for 'RPUSH' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << '\n';
  }
  return engine.rpush(args);
}

std::string lpop_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 2)
    return "-ERR wrong number of arguments for 'LPOP' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }
  return engine.lpop(args);
}

std::string rpop_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 2)
    return "-ERR wrong number of arguments for 'RPOP' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << '\n';
  }
  return engine.rpop(args);
}

std::string llen_handler(std::vector<std::string> &args, RedisWrapper &engine) {
  if (args.size() != 3)
    return "-ERR wrong number of arguments for 'LLEN' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << '\n';
  }
  return engine.llen(args);
}

std::string lrange_handler(std::vector<std::string> &args,
                           RedisWrapper &engine) {
  if (args.size() != 4)
    return "-ERR wrong number of arguments for 'LRANGE' command\r\n";
  if (G_DEBUG) {
    LOG_INFO << "command is: " << args[0] << " " << args[1] << " " << args[2]
             << " " << args[3] << '\n';
  }
  return engine.lrange(args);
}
