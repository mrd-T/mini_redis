#include "../../include/config/config.h"

#include <iostream> // For std::cerr, std::endl

// Private helper to set all default values
void TomlConfig::setDefaultValues() {
  // --- LSM Core ---
  lsm_tol_mem_size_limit_ = 67108864; // Default: 64 * 1024 * 1024
  lsm_per_mem_size_limit_ = 4194304;  // Default: 4 * 1024 * 1024
  lsm_block_size_ = 32768;            // Default: 32 * 1024
  lsm_sst_level_ratio_ = 4;           // Default: 4

  // --- LSM Cache ---
  lsm_block_cache_capacity_ = 1024; // Default: 1024
  lsm_block_cache_k_ = 8;           // Default: 8

  // --- Redis Headers/Separators ---
  redis_expire_header_ = "REDIS_EXPIRE_";
  redis_hash_value_preffix_ = "REDIS_HASH_VALUE_";
  redis_field_prefix_ = "REDIS_FIELD_";
  redis_field_separator_ = '$';
  redis_list_separator_ = '#';
  redis_sorted_set_prefix_ = "REDIS_SORTED_SET_";
  redis_sorted_set_score_len_ = 32;
  redis_set_prefix_ = "REDIS_SET_";

  // --- Bloom Filter ---
  bloom_filter_expected_size_ = 65536;
  bloom_filter_expected_error_rate_ = 0.1;
}

// Constructor implementation
TomlConfig::TomlConfig(const std::string &filePath) {
  // Initialize member variables with default values upon creation
  if (filePath.empty()) {
    setDefaultValues();
  } else {
    // Load configuration from the specified file
    try {
      loadFromFile(filePath);
    } catch (const std::exception &err) {
      std::cerr << "Error loading configuration: " << err.what() << std::endl;
      setDefaultValues();
    }
  }
}

// Method to load configuration from a TOML file implementation
bool TomlConfig::loadFromFile(const std::string &filePath) {
  // Reset to defaults before loading to ensure a clean state
  setDefaultValues();

  try {
    // Use toml::parse_file to load and parse the TOML file
    auto config = toml::parse(filePath);

    // --- Load LSM Core ---
    // Use value_or to get the value or the current default if the key or table
    // is missing
    auto core_config = config["lsm"]["core"];

    lsm_tol_mem_size_limit_ =
        core_config.at("LSM_TOL_MEM_SIZE_LIMIT").as_integer();
    lsm_per_mem_size_limit_ =
        core_config.at("LSM_PER_MEM_SIZE_LIMIT").as_integer();
    lsm_block_size_ = core_config.at("LSM_BLOCK_SIZE").as_integer();
    lsm_sst_level_ratio_ = core_config.at("LSM_SST_LEVEL_RATIO").as_integer();

    // --- Load LSM Cache ---
    auto cache_config = config["lsm"]["cache"];

    lsm_block_cache_capacity_ =
        cache_config.at("LSM_BLOCK_CACHE_CAPACITY").as_integer();
    lsm_block_cache_k_ = cache_config.at("LSM_BLOCK_CACHE_K").as_integer();

    // --- Load Redis Headers/Separators ---
    auto redis_config = config["redis"];

    redis_expire_header_ = redis_config.at("REDIS_EXPIRE_HEADER").as_string();
    redis_hash_value_preffix_ =
        redis_config.at("REDIS_HASH_VALUE_PREFFIX").as_string();
    redis_field_prefix_ = redis_config.at("REDIS_FIELD_PREFIX").as_string();

    redis_field_separator_ =
        redis_config.at("REDIS_FIELD_SEPARATOR").as_string()[0];

    redis_list_separator_ =
        redis_config.at("REDIS_LIST_SEPARATOR").as_string()[0];

    redis_sorted_set_prefix_ =
        redis_config.at("REDIS_SORTED_SET_PREFIX").as_string();

    redis_sorted_set_score_len_ =
        redis_config.at("REDIS_SORTED_SET_SCORE_LEN").as_integer();

    redis_set_prefix_ = redis_config.at("REDIS_SET_PREFIX").as_string();

    // --- Load Bloom Filter ---
    auto bloom_config = config["bloom_filter"];

    bloom_filter_expected_size_ =
        bloom_config.at("BLOOM_FILTER_EXPECTED_SIZE").as_integer();

    bloom_filter_expected_error_rate_ =
        bloom_config.at("BLOOM_FILTER_EXPECTED_ERROR_RATE").as_floating();

    std::cout << "Configuration loaded successfully from " << filePath
              << std::endl;
    return true;

  } catch (const std::exception &err) {
    // Handle other potential exceptions
    std::cerr
        << "An unexpected error occurred while loading configuration from "
        << filePath << ": " << err.what() << std::endl;
    // Configuration remains at default values
    return false; // Loading failed
  }
}

// --- Getter Methods Implementations ---

long long TomlConfig::getLsmTolMemSizeLimit() const {
  return lsm_tol_mem_size_limit_;
}
long long TomlConfig::getLsmPerMemSizeLimit() const {
  return lsm_per_mem_size_limit_;
}
int TomlConfig::getLsmBlockSize() const { return lsm_block_size_; }
int TomlConfig::getLsmSstLevelRatio() const { return lsm_sst_level_ratio_; }

int TomlConfig::getLsmBlockCacheCapacity() const {
  return lsm_block_cache_capacity_;
}
int TomlConfig::getLsmBlockCacheK() const { return lsm_block_cache_k_; }

const std::string &TomlConfig::getRedisExpireHeader() const {
  return redis_expire_header_;
}
const std::string &TomlConfig::getRedisHashValuePreffix() const {
  return redis_hash_value_preffix_;
}
const std::string &TomlConfig::getRedisFieldPrefix() const {
  return redis_field_prefix_;
}
char TomlConfig::getRedisFieldSeparator() const {
  return redis_field_separator_;
}
char TomlConfig::getRedisListSeparator() const { return redis_list_separator_; }
const std::string &TomlConfig::getRedisSortedSetPrefix() const {
  return redis_sorted_set_prefix_;
}
int TomlConfig::getRedisSortedSetScoreLen() const {
  return redis_sorted_set_score_len_;
}
const std::string &TomlConfig::getRedisSetPrefix() const {
  return redis_set_prefix_;
}

int TomlConfig::getBloomFilterExpectedSize() const {
  return bloom_filter_expected_size_;
}
double TomlConfig::getBloomFilterExpectedErrorRate() const {
  return bloom_filter_expected_error_rate_;
}

const TomlConfig &TomlConfig::getInstance(const std::string &config_path) {
  // Static instance ensures single creation upon first call

  static const TomlConfig instance(config_path);
  return instance;
}