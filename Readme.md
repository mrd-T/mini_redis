`Toni-LSM` is a personal toy project to implement a simple kv database from scratch, using lsm-tree as the storage engine. The project manager uses [xmake](https://xmake.io/). The project is inspired by [mini-lsm](https://github.com/skyzh/mini-lsm), [tinykv](https://github.com/talent-plan/tinykv) and [leveldb](https://github.com/google/leveldb). The project is partly compatible with the [Redis Resp protocol](https://redis.io/docs/latest/develop/reference/protocol-spec/), so it can be used as a redis backend and relpace `redis-server`(Just for fun).

The developing posts can be found in my [blog](https://tonixwd.github.io/categories/%E9%A1%B9%E7%9B%AE/) or [zhihu](https://www.zhihu.com/column/c_1867534978344161280).

## Build Configuration

The project uses xmake as the build system. Below is the xmake configuration for building the project and running tests:

1. Compile the project
```bash
xmake
```

2. Run the example program or test
```bash
xmake run example
xmake run test_lsm
```

3. Install the shared library
```bash
xmake install --root lsm_shared
```

# Usage

## Use as a library

Here is a simple example demonstrating how to use the LSM Tree for basic key-value operations:

```cpp
#include "../include/lsm/engine.h"
#include <iostream>
#include <string>

int main() {
  // Create an LSM instance and specify the data storage directory
  LSM lsm("example_data");

  // Insert data
  lsm.put("key1", "value1");
  lsm.put("key2", "value2");
  lsm.put("key3", "value3");

  // Query data
  auto value1 = lsm.get("key1");
  if (value1.has_value()) {
    std::cout << "key1: " << value1.value() << std::endl;
  } else {
    std::cout << "key1 not found" << std::endl;
  }

  // Update data
  lsm.put("key1", "new_value1");
  auto new_value1 = lsm.get("key1");
  if (new_value1.has_value()) {
    std::cout << "key1: " << new_value1.value() << std::endl;
  } else {
    std::cout << "key1 not found" << std::endl;
  }

  // Delete data
  lsm.remove("key2");
  auto value2 = lsm.get("key2");
  if (value2.has_value()) {
    std::cout << "key2: " << value2.value() << std::endl;
  } else {
    std::cout << "key2 not found" << std::endl;
  }

  // Iterate over all data
  std::cout << "All key-value pairs:" << std::endl;
  for (auto it = lsm.begin(); it != lsm.end(); ++it) {
    std::cout << it->first << ": " << it->second << std::endl;
  }

  return 0;
}
```

## Use to replace redis-server
Now the project only partly compatible with the Redis Resp protocol, you can check `TODO` for the supported Redis commands.
```bash
xmake run server
```
Then you can use redis-cli to connect to the server:

![redis-example](./doc/redis-example.png)

## Redis Benchmark
We use the official tool `redis-benchmark` to test the performance of the wrapper redis server. The QPS of commonly used commands are relatively high, considering its IO between memroy and disk.
> The testing environment is: Win11 WSL Ubuntu 22.04, 32GB 6000 RAM, Intel 12600K. 
```bash
(base) ➜  ~ redis-benchmark -h 127.0.0.1 -p 6379 -c 100 -n 100000 -q -t SET,GET,INCR,SADD,HSET,ZADD
WARNING: Could not fetch server CONFIG
SET: 60277.27 requests per second, p50=1.423 msec
GET: 60204.70 requests per second, p50=1.431 msec
INCR: 124843.95 requests per second, p50=0.551 msec
SADD: 59880.24 requests per second, p50=1.447 msec
HSET: 60827.25 requests per second, p50=1.431 msec
ZADD: 33422.46 requests per second, p50=2.783 msec
```
> Besides, the QPS of `lpush/rpush` is so slow that its design needs to be optimized.

**The project is under development, and the current version is not stable.**

# TODO
- [x] SkipList
  - [x] get/put/remove
  - [x] iterator
  - [x] Range Query
- [x] MemTable
  - [x] Iterator
  - [x] Merge
  - [x] Range Query
  - [x] flush to sst
- [ ] SST
  - [x] Encode/Decode
  - [x] Iterator
  - [x] Query
  - [x] Range Query
  - [ ] Compact
- [ ] Wal
- [ ] Transaction
  - [ ] MVCC
  - [ ] Snapshot
- [x] Redis
  - [x] Fundamental KV Operations
    - [x] set
    - [x] get
    - [x] ttl
    - [x] expire
  - [x] Hash Operations
    - [x] hset
    - [x] hget
    - [x] hkeys
    - [x] hdel
  - [x] List Operations
    - [x] lpush/rpush
    - [x] lpop/rpop
    - [x] llen
    - [x] lrange
  - [x] ZSet
    - [x] ZADD
    - [x] ZREM
    - [x] ZCARD
    - [x] ZRANGE
    - [x] ZSCORE
    - [x] ZRANK
    - [x] ZINCRBY
  - [x] Set
    - [x] SADD
    - [x] SREM
    - [x] SISMEMBER
    - [x] SMEMBERS
    - [x] SCARD
  - [x] IO Operations
    - [x] FLUSHALL
    - [x] SAVE

> Only commonly used redis commands are supported. The other implementations can refer to `src/redis_wrapper/redis_wrapper.cpp`. If you need more commands, please submit a pull request. 

## License

This project is licensed under the MIT License.