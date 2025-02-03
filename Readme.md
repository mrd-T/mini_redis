`Toni-LSM` is a personal toy project to implement a simple kv database from scratch, using lsm-tree as the storage engine. The project manager uses [xmake](https://xmake.io/). The project is inspired by [mini-lsm](https://github.com/skyzh/mini-lsm), [tinykv](https://github.com/talent-plan/tinykv) and [leveldb](https://github.com/google/leveldb).

The developing posts can be found in my [blog](https://tonixwd.github.io/categories/%E9%A1%B9%E7%9B%AE/) or [zhihu](https://www.zhihu.com/column/c_1867534978344161280).

# Usage

## Example Code

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
    std::cout << (*it).first << ": " << (*it).second << std::endl;
  }

  return 0;
}
```

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

**The project is under development, and the current version is not stable.**

# TODO
- [x] SkipList
  - [x] get/put/remove
  - [x] iterator
- [ ] MemTable
  - [x] Iterator
  - [x] Merge
  - [ ] Range Query
  - [x] flush to sst
- [ ] SST
  - [x] Encode/Decode
  - [x] Iterator
  - [x] Query
  - [ ] Range Query
  - [ ] Compact
- [ ] Wal
- [ ] Transaction
  - [ ] MVCC
  - [ ] Snapshot
