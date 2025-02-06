#pragma once

#define LSM_TOL_MEM_SIZE_LIMIT (64 * 1024 * 1024) // 内存表的大小限制, 64MB
#define LSM_PER_MEM_SIZE_LIMIT (4 * 1024 * 1024) // 内存表的大小限制, 4MB
#define LSM_BLOCK_SIZE (16 * 1024)               // BLOCK的大小, 16KB

// // 测试时使用的小批量数据, 测试时可以注释上面的定义
// #define LSM_TOL_MEM_SIZE_LIMIT (64 * 1024) // 内存表的大小限制, 64KB
// #define LSM_PER_MEM_SIZE_LIMIT (4 * 1024)  // 内存表的大小限制, 4KB
// #define LSM_BLOCK_SIZE (1024)              // BLOCK的大小, 1KB

#define LSMmm_BLOCK_CACHE_CAPACITY 1024 // 缓存池的块缓存容量
#define LSMmm_BLOCK_CACHE_K 8           // 缓存池的LRU-K的K值