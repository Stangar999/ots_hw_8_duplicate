#pragma once

#include <boost/algorithm/hex.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/crc.hpp>
#include <boost/flyweight.hpp>
#include <boost/regex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <filesystem>
#include <map>

#include "common.h"
//------------------------------------------------------------------------------
class DuplicateSearcher {
  using path = std::filesystem::path;
  using BoostBimap = boost::bimap<
      boost::bimaps::unordered_multiset_of<std::string>,
      boost::bimaps::unordered_set_of<FileParams, FileParams::Hash>>;

 public:
  DuplicateSearcher() {
  }
  /// ищет дубли и возвращает их
  std::vector<std::vector<std::string>> FindDupl(SearchParams search_params);

 private:
  /// рекурсия для поиска дуплей (в глубину)
  /// обходит директорию сохраняя подходящие пути
  void FindDupl(const std::filesystem::path& path, int deep);
  /// проверка путя в списке исключения
  bool IsExclude(path path) const;
  /// проверка размера файла на соответствие параметра
  bool FileSizeMoreMin(path path) const;
  /// проверка имени файла на соответствие параметра
  bool MatchMasks(path path) const;
  /// проверка глубины файла на соответствие параметра
  bool DeepLessMax(int deep) const;
  /// получения хеша по куску строки
  std::string BlockToString(const char* block) const;
  /// проверяет состояние файла и возвращает хеш для файла от начала до текущей
  /// позиции
  std::string GetHash(FileParams& f_p) const;
  /// заносит все иследуемые фалы в контейнер для хеша для дальнейшеего
  /// рекурсивного вызова GetHash
  void CrateHashes(BoostBimap& hashes, const path& path) const;
  /// обновляем хеши и выставляем флаг если обновлений не было
  bool UpdateHashes(BoostBimap& hashes) const;
  /// удаляем уникальный хеши и файлы из исследуемых
  void ExludeUniqFile(BoostBimap& hashes) const;

  /// обрабатываем подходящие пути находя хеш, удаляя уникальные и возвращая
  /// дубликаты
  std::vector<std::vector<std::string>> СountingDupl();

  SearchParams _search_params;
  std::vector<path> _researched_file;
  std::map<int, std::vector<boost::flyweights::flyweight<path>>> _duplicates;
};
//------------------------------------------------------------------------------
