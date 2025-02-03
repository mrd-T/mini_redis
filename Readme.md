A personal toy project to implement a simple kv database from scratch, using lsm-tree as the storage engine. The project manager uses [xmake](https://xmake.io/)

The posts can be found in my [blog](https://tonixwd.github.io/categories/%E9%A1%B9%E7%9B%AE/) or [zhihu](https://www.zhihu.com/column/c_1867534978344161280)

The project is inspired by [mini-lsm](https://github.com/skyzh/mini-lsm), [tinykv](https://github.com/talent-plan/tinykv) and [leveldb](https://github.com/google/leveldb)

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

