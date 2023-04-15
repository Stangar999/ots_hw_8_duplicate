#pragma once

#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <vector>

#include "common.h"

namespace {
namespace po = boost::program_options;
//------------------------------------------------------------------------------
// создает описание возможных параметров
static po::options_description CreateDescription() {
  po::options_description desc{"Options"};
  desc.add_options()("help,h", "This screen")(
      "scan_dir,p",
      po::value<std::vector<std::string>>()->multitoken()->required(),
      "path to the scanned directory")(
      "exlude_dir,e", po::value<std::vector<std::string>>()->multitoken(),
      "path to the exclude scanned directory")(
      "deep_scan,d", po::value<uint32_t>()->default_value(0), "scaning depth")(
      "min_fsize,s", po::value<uint32_t>()->default_value(1),
      "minimum file size")(
      "file_masks,m", po::value<std::vector<std::string>>()->multitoken(),
      "file masks")("block_size,b", po::value<uint32_t>()->default_value(1),
                    "the size of a scan block at a time in a file")(
      "crc32", "Use crc32 algorithm to compare")(
      "md5", "Use md5 algorithm to compare");
  return desc;
}
//------------------------------------------------------------------------------
static po::variables_map ParseCommandLine(int argc, const char* argv[],
                                          const po::options_description& desc) {
  po::variables_map vm;
  try {
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    std::cout << desc << '\n';
  }
  return vm;
}
//------------------------------------------------------------------------------
// парсит словарь со считанными параметрами и складывает в структуру
static SearchParams CreateSearchParams(const po::variables_map& vm) {
  auto getMany = [](const po::variable_value& variable, auto& dest) {
    const auto& container = variable.as<std::vector<std::string>>();
    dest.assign(container.begin(), container.end());
  };
  SearchParams search_params;
  if (vm.count("scan_dir")) {
    getMany(vm["scan_dir"], search_params.include_paths);
    if (vm.count("exlude_dir")) {
      getMany(vm["exlude_dir"], search_params.exclude_paths);
    }
    if (vm.count("deep_scan")) {
      search_params.deep = vm["deep_scan"].as<uint32_t>();
    }
    if (vm.count("min_fsize")) {
      search_params.min_file_size = vm["min_fsize"].as<uint32_t>();
    }
    if (vm.count("file_masks")) {
      getMany(vm["file_masks"], search_params.masks);
    }
    if (vm.count("block_size")) {
      search_params.block_size = vm["block_size"].as<uint32_t>();
    }
    if (vm.count("crc32")) {
      search_params.hash_method = HashMethod::CRC32;
    }
    if (vm.count("md5")) {
      search_params.hash_method = HashMethod::MD5;
    }
  }
  return search_params;
}
//------------------------------------------------------------------------------
}  // namespace
