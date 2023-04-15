#pragma once

#include <boost/flyweight.hpp>
#include <filesystem>
#include <functional>
#include <vector>

namespace fw = boost::flyweights;
namespace fs = std::filesystem;
//------------------------------------------------------------------------------
enum class HashMethod {
  //  NOINIT,
  CRC32,
  MD5,
};

//------------------------------------------------------------------------------
struct SearchParams {
 private:
  //  struct include_paths_t {};
  //  struct exclude_paths_t {};
  enum class include_paths_t {};
  enum class exclude_paths_t {};

 public:
  std::vector<fw::flyweight<fs::path, fw::tag<include_paths_t>>> include_paths;
  std::vector<fw::flyweight<fs::path, fw::tag<exclude_paths_t>>> exclude_paths;
  uint32_t deep = 0;
  uint32_t min_file_size = 0;
  std::vector<std::string_view> masks;
  uint32_t block_size = 0;
  HashMethod hash_method = HashMethod::CRC32;
};
//------------------------------------------------------------------------------
struct FileParams {
  std::filesystem::path path;
  size_t curr_pos = 0;
  bool is_end = false;
  bool is_bad = false;

  friend bool operator==(const FileParams& lth, const FileParams& rth) {
    return lth.path == rth.path;
  }
  struct Hash {
    size_t operator()(const FileParams& f_p) const {
      return std::hash<std::string>{}(f_p.path.string());
    }
  };
};
//------------------------------------------------------------------------------
