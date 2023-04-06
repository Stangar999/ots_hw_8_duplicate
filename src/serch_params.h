#pragma once

#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <vector>

#include "common.h"

namespace {
namespace po = boost::program_options;

// const string scan_dir("scan_dir"s);
// const string exlude_dir("exlude_dir"s);
// const string deep_scan("deep_scan");
// const string min_fsize("min_fsize");
// const string file_masks("file_masks");
// const string block_size("block_size");
// const string crc32("crc32");
// const string md5("md5");
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
    std::cerr << e.what() << std::endl;
  }
  return vm;
}
//------------------------------------------------------------------------------
// парсит словарь со считанными параметрами и складывает в структуру
static SearchParams CreateSearchParams(const po::variables_map& vm) {
  SearchParams search_params;
  if (vm.count("scan_dir")) {
    // TODO копии!
    search_params.include_paths.assign(
        vm["scan_dir"].as<std::vector<std::string>>().begin(),
        vm["scan_dir"].as<std::vector<std::string>>().end());
    if (vm.count("exlude_dir")) {
      // TODO копии!
      search_params.exclude_paths.assign(
          vm["exlude_dir"].as<std::vector<std::string>>().begin(),
          vm["exlude_dir"].as<std::vector<std::string>>().end());
    }
    if (vm.count("deep_scan")) {
      search_params.deep = vm["deep_scan"].as<uint32_t>();
    }
    if (vm.count("min_fsize")) {
      search_params.min_file_size = vm["min_fsize"].as<uint32_t>();
    }
    if (vm.count("file_masks")) {
      // TODO копии!
      search_params.masks.assign(
          vm["file_masks"].as<std::vector<std::string>>().begin(),
          vm["file_masks"].as<std::vector<std::string>>().end());
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
