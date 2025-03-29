#pragma once

#include "../wal/record.h"
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class LSMEngine;
class TranManager;

class TranContext {
public:
  TranContext(uint64_t tranc_id, std::shared_ptr<LSMEngine> engine,
              std::shared_ptr<TranManager> tranManager);
  void put(const std::string &key, const std::string &value);
  void remove(const std::string &key);
  std::optional<std::string> get(const std::string &key);
  bool commit();
  void abort();

  std::shared_ptr<LSMEngine> engine_;
  std::shared_ptr<TranManager> tranManager_;
  uint64_t tranc_id_;
  std::vector<Record> operations;
  std::unordered_map<std::string, std::string> temp_map_;
  bool isCommited = false;
  bool isAborted = false;
};

class TranManager : public std::enable_shared_from_this<TranManager> {
public:
  TranManager(std::string data_dir);
  ~TranManager();
  uint64_t getNextTransactionId();
  void set_engine(std::shared_ptr<LSMEngine> engine);
  std::shared_ptr<TranContext> new_tranc();

private:
  std::string get_tranc_id_file_path();

private:
  std::shared_ptr<LSMEngine> engine_;
  std::string data_dir_;
  mutable std::mutex mutex_;
  std::atomic<uint64_t> nextTransactionId_;
  std::map<uint64_t, std::shared_ptr<TranContext>> activeTrans_;
};
