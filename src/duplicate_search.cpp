#include "duplicate_search.h"

#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
//------------------------------------------------------------------------------
std::vector<std::vector<std::string>> DuplicateSearcher::FindDupl(
    SearchParams search_params) {
  _search_params = std::move(search_params);
  int deep = 0;
  for (const auto& path : _search_params.include_paths) {
    if (fs::is_directory(path)) {
      FindDupl(path, deep);
    }
  }
  return СountingDupl();
}
//------------------------------------------------------------------------------
void DuplicateSearcher::FindDupl(const fs::path& path, int deep) {
  if (!DeepLessMax(deep)) {
    return;
  }
  if (!fs::exists(path)) {
    return;
  }
  if (IsExclude(path)) {
    return;
  }
  fs::directory_iterator begin(path);
  fs::directory_iterator end;
  for (; begin != end; ++begin) {
    fs::file_status status = begin->status();
    if (fs::is_directory(status)) {
      FindDupl(begin->path(), deep + 1);
    } else if (fs::is_regular_file(status)) {
      if (FileSizeMoreMin(begin->path()) && MatchMasks(begin->path())) {
        _researched_file.emplace_back(begin->path());
      }
    }
  }
}
//------------------------------------------------------------------------------
bool DuplicateSearcher::IsExclude(path checkeble_path) const {
  for (const auto& ex_path : _search_params.exclude_paths) {
    if (checkeble_path == ex_path) {
      return true;
    }
  }
  return false;
}
//------------------------------------------------------------------------------
bool DuplicateSearcher::FileSizeMoreMin(path path) const {
  return fs::file_size(path) >= _search_params.min_file_size;
}
//------------------------------------------------------------------------------
bool DuplicateSearcher::MatchMasks(path path) const {
  if (_search_params.masks.empty()) {
    return true;
  }
  std::string filename = path.filename().string();
  for (const auto& mask : _search_params.masks) {
    boost::regex r(mask.data());
    if (boost::regex_match(filename, r)) {
      return true;
    }
  }
  return false;
}
//------------------------------------------------------------------------------
bool DuplicateSearcher::DeepLessMax(int deep) const {
  return deep <= _search_params.deep;
}
//------------------------------------------------------------------------------
std::vector<std::vector<std::string>> DuplicateSearcher::СountingDupl() {
  if (_researched_file.size() < 2) {
    return {};
  }

  BoostBimap hashes;
  for (const auto& path : _researched_file) {
    CrateHashes(hashes, path);
  }
  ExludeUniqFile(hashes);

  for (bool complete_compare = false; complete_compare != true;) {
    complete_compare = UpdateHashes(hashes);
    ExludeUniqFile(hashes);
  }

  std::vector<std::vector<std::string>> dupl_path;
  std::set<std::string> prev_added_keys;
  for (const auto& [key, _] : hashes) {
    auto [it, is_add] = prev_added_keys.insert(key);
    if (!is_add) {
      continue;
    }
    auto [l_it, r_it] = hashes.left.equal_range(key);
    std::vector<std::string> eq_path;
    for (; l_it != r_it; ++l_it) {
      eq_path.push_back(l_it->second.path.string());
    }
    dupl_path.push_back(eq_path);
  }
  return dupl_path;
}
//------------------------------------------------------------------------------
std::string DuplicateSearcher::BlockToString(const char* block) const {
  std::string result;
  switch (_search_params.hash_method) {
    case HashMethod::MD5: {
      boost::uuids::detail::md5 hash;
      boost::uuids::detail::md5::digest_type digest;
      hash.process_bytes(block, _search_params.block_size);
      hash.get_digest(digest);
      const auto charDigest = reinterpret_cast<const char*>(&digest);
      boost::algorithm::hex(
          charDigest,
          charDigest + sizeof(boost::uuids::detail::md5::digest_type),
          std::back_inserter(result));
      break;
    }
    case HashMethod::CRC32: {
      boost::crc_32_type crc;
      crc.process_bytes(block, _search_params.block_size);
      result = std::to_string(crc.checksum());
      break;
    }
  }
  return result;
}
//------------------------------------------------------------------------------
std::string DuplicateSearcher::GetHash(FileParams& f_p) const {
  std::ifstream file(f_p.path);
  if (!file.is_open()) {
    f_p.is_bad = true;
    return "";
  }
  const size_t block_size = _search_params.block_size;
  // перемечаем начало считывание на последнее место остановки
  file.ignore(f_p.curr_pos);
  // массив для заполнения
  std::string block(block_size, '\0');
  file.read(block.data(), block_size);
  // обновляем позицию докуда считали
  f_p.curr_pos += file.gcount();
  // если не удалось полностью считать запланированный блок
  if (file.gcount() < block_size) {
    // то если файл закончился, заполняем нулями оставшийся хвост
    // и устанавливаем флаг конца
    if (file.eof()) {
      std::fill(block.data() + file.gcount(), block.data() + block_size, '\0');
      f_p.is_end = true;
      // иначе устанавливаем флаг плохого файла
    } else {
      // std::ifstream::failure("fail read");
      f_p.is_bad = true;
      return "";
    }
  }
  return BlockToString(block.data());
}
//------------------------------------------------------------------------------
void DuplicateSearcher::CrateHashes(BoostBimap& hashes,
                                    const path& path) const {
  std::string hash;
  FileParams f_p{path, 0, false};

  hash = GetHash(f_p);
  if (!f_p.is_bad) {
    // вставляем обновленный хеш с элементом
    hashes.insert({hash, f_p});
  }
}
//------------------------------------------------------------------------------
bool DuplicateSearcher::UpdateHashes(BoostBimap& hashes) const {
  std::string hash;
  FileParams f_p_tmp;
  BoostBimap new_hashes;
  bool complete_compare = true;
  for (const auto& [key, f_p] : hashes) {
    if (f_p.is_end) {
      continue;
    }
    // уникальные удалены а все не уникальные проверены до конца
    complete_compare = true;
    f_p_tmp = f_p;
    hash = key;
    hash += GetHash(f_p_tmp);
    if (!f_p_tmp.is_bad) {
      // вставляем обновленный хеш с элементом
      new_hashes.insert({std::move(hash), std::move(f_p_tmp)});
    }
  }
  hashes.clear();
  hashes = std::move(new_hashes);
  return complete_compare;
}
//------------------------------------------------------------------------------
void DuplicateSearcher::ExludeUniqFile(BoostBimap& hashes) const {
  std::vector<std::string> unique_files;
  for (const auto& [key, _] : hashes) {
    if (hashes.left.count(key) == 1) {
      unique_files.push_back(key);
    }
  }

  for (const std::string& key : unique_files) {
    hashes.left.erase(key);
  }
}
//------------------------------------------------------------------------------
