#include <gtest/gtest.h>

#include <boost/program_options.hpp>
#include <filesystem>

#include "common.h"
#include "serch_params.h"

using namespace std;
namespace po = boost::program_options;

TEST(ParseCommandLine, ALL) {
  int argc = 17;
  const char* argv[]{"C:/Users/locki",
                     "-p",
                     "C:/Users/locki/Documents/BOOST",
                     "C:/Users/locki/Project",
                     "-e",
                     "E:/Users/Project",
                     "E:/Project",
                     "-d",
                     "3",
                     "-s",
                     "2",
                     "-b",
                     "128",
                     "-m",
                     "*.txt",
                     "*.cpp",
                     "--md5"};

  po::options_description desc = CreateDescription();
  po::variables_map vm = ParseCommandLine(argc, argv, CreateDescription());
  if (vm.count("help")) {
    std::cout << desc << '\n';
  }
  SearchParams search_params = CreateSearchParams(vm);
  ASSERT_EQ(search_params.include_paths[0],
            std::filesystem::path("C:/Users/locki/Documents/BOOST"));
  ASSERT_EQ(search_params.include_paths[1],
            std::filesystem::path("C:/Users/locki/Project"));

  ASSERT_EQ(search_params.exclude_paths[0],
            std::filesystem::path("E:/Users/Project"));
  ASSERT_EQ(search_params.exclude_paths[1],
            std::filesystem::path("E:/Project"));

  ASSERT_EQ(search_params.deep, 3);
  ASSERT_EQ(search_params.min_file_size, 2);
  ASSERT_EQ(search_params.block_size, 128);

  ASSERT_EQ(search_params.masks[0], std::string("*.txt"));
  ASSERT_EQ(search_params.masks[1], std::string("*.cpp"));

  ASSERT_EQ(search_params.hash_method, HashMethod::MD5);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
