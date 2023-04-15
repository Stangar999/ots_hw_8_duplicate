#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <vector>

#include "common.h"
#include "duplicate_search.h"
#include "serch_params.h"

using namespace std;
namespace po = boost::program_options;
//------------------------------------------------------------------------------
int main(int argc, const char* argv[]) {
  po::options_description desc = CreateDescription();
  po::variables_map vm = ParseCommandLine(argc, argv, desc);
  if (vm.count("help")) {
    std::cout << desc << '\n';
    return 0;
  }

  SearchParams search_params = CreateSearchParams(vm);
  DuplicateSearcher duplicate_searcher;
  std::vector<std::vector<std::string>> duplicates;
  duplicates = duplicate_searcher.FindDupl(std::move(search_params));
  // вывод дубликатов
  for (const auto& group_dupl : duplicates) {
    for (const auto& path : group_dupl) {
      std::cout << path << std::endl;
    }
    std::cout << std::endl;
  }
  return 0;
}
//------------------------------------------------------------------------------
