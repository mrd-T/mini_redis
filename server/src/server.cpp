#include "../../include/redis_wrapper/redis_wrapper.h"
#include "../include/handler.h"
#include <cstddef>
#include <iostream>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <string>
#include <unordered_map>
#include <vector>

using namespace muduo;
using namespace muduo::net;

class RedisServer {
public:
  RedisServer(EventLoop *loop, const InetAddress &listenAddr)
      : server_(loop, listenAddr, "RedisServer"), redis("example_db") {
    server_.setConnectionCallback(
        std::bind(&RedisServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(
        std::bind(&RedisServer::onMessage, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
  }

  void start() { server_.start(); }

  // private:
  void onConnection(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
      LOG_INFO << "Connection from " << conn->peerAddress().toIpPort();
    } else {
      LOG_INFO << "Connection closed from " << conn->peerAddress().toIpPort();
    }
  }

  void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO << "Received message at " << time.toString() << ":\n" << msg;

    // 解析并处理请求
    std::string response = handleRequest(msg);
    conn->send(response);
  }

  std::string handleRequest(const std::string &request) {
    size_t pos = 0;

    if (request.empty() || request[pos] != '*') {
      return "-ERR Protocol error: expected '*'\r\n";
    }

    int numElements = 0;
    try {
      numElements = std::stoi(request.substr(pos + 1)); // 跳过 '*'
    } catch (const std::exception &) {
      return "-ERR Protocol error: invalid number of elements\r\n";
    }
    pos = request.find('\n', pos) + 1; // 跳过 '\r\n'

    LOG_INFO << "request: " << request << '\n';
    LOG_INFO << "Number of elements: " << numElements << '\n';

    std::vector<std::string> args;

    for (int i = 0; i < numElements; ++i) {
      if (pos >= request.size() || request[pos] != '$') {
        LOG_INFO << "pos = " << pos << ", i = " << i
                 << ", last args = " << args.back() << '\n';
        LOG_INFO << "-ERR Protocol error: expected '$'\r\n";
        return "-ERR Protocol error: expected '$'\r\n";
      }

      int len = 0;
      std::string value_len;
      int next_n_pos;
      try {
        next_n_pos = request.find('\n', pos);
        len = std::stoi(request.substr(pos + 1)); // 跳过 '$'
      } catch (const std::exception &) {
        LOG_INFO << "-ERR Protocol error: invalid bulk string length\r\n";
        return "-ERR Protocol error: invalid bulk string length\r\n";
      }
      pos = next_n_pos + 1; // 跳过 '$' 值 \r\n
      if (pos + len > request.size()) {
        LOG_INFO << "-ERR Protocol error: bulk string length exceeds request "
                    "size\r\n";
        return "-ERR Protocol error: bulk string length exceeds request "
               "size\r\n";
      }
      args.push_back(request.substr(pos, len));
      next_n_pos = request.find('\n', pos);
      pos = next_n_pos + 1; // 跳过数据和/r/n
    }

    LOG_INFO << "Request: ";
    for (const auto &arg : args) {
      LOG_INFO << arg << " ";
    }
    LOG_INFO << '\n';

    // 处理命令
    switch (string2Ops(args[0])) {
    case OPS::FLUSHALL:
      return flushall_handler(redis);
    case OPS::SAVE:
      return save_handler(redis);
    case OPS::SET:
      return set_handler(args, redis);
    case OPS::GET:
      return get_handler(args, redis);
    case OPS::DEL:
      return del_handler(args, redis);
    case OPS::INCR:
      return incr_handler(args, redis);
    case OPS::DECR:
      return decr_handler(args, redis);
    case OPS::EXPIRE:
      return expire_handler(args, redis);
    case OPS::TTL:
      return ttl_handler(args, redis);
    case OPS::HSET:
      return hset_handler(args, redis);
    case OPS::HGET:
      return hget_handler(args, redis);
    case OPS::HDEL:
      return hdel_handler(args, redis);
    case OPS::HKEYS:
      return hkeys_handler(args, redis);
    case OPS::LLEN:
      return llen_handler(args, redis);
    case OPS::LPUSH:
      return lpush_handler(args, redis);
    case OPS::RPUSH:
      return rpush_handler(args, redis);
    case OPS::LPOP:
      return lpop_handler(args, redis);
    case OPS::RPOP:
      return rpop_handler(args, redis);
    case OPS::LRANGE:
      return lrange_handler(args, redis);
    case OPS::ZADD:
      return zadd_handler(args, redis);
    case OPS::ZCARD:
      return zcard_handler(args, redis);
    case OPS::ZINCRBY:
      return zincrby_handler(args, redis);
    case OPS::ZRANGE:
      return zrange_handler(args, redis);
    case OPS::ZRANK:
      return zrank_handler(args, redis);
    case OPS::ZSCORE:
      return zscore_handler(args, redis);
    case OPS::ZREM:
      return zrem_handler(args, redis);
    case OPS::SADD:
      return sadd_handler(args, redis);
    case OPS::SMEMBERS:
      return smembers_handler(args, redis);
    case OPS::SCARD:
      return scard_handler(args, redis);
    case OPS::SISMEMBER:
      return sismember_handler(args, redis);
    case OPS::SREM:
      return srem_handler(args, redis);
    default:
      return "-ERR unknown command '" + args[0] + "'\r\n";
    }
  }

  TcpServer server_;
  RedisWrapper redis; // 简单的键值存储
};

int main() {
  EventLoop loop;
  InetAddress listenAddr(6379); // Redis默认端口
  RedisServer server(&loop, listenAddr);

  server.start();
  loop.loop(); // 进入事件循环
  // DEBUG
  // server.handleRequest(
  //     "*3\r\n$3\r\nSET\r\n$6\r\npasswd\r\n$12\r\ntmdgnnwscjl\r\n");
}