#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "argparse.hpp"
#include "core/parser.h"

using namespace parser;
using namespace std;

int main(int argc, char* argv[]) {
  argparse::ArgumentParser program("test");

  program.add_argument("--build")
      .help("build root dir")
      .default_value(false)
      .implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  if (program["--build"] == true) {
    std::cout << "Building" << std::endl;
  }

  // parser_class parser_class;
  // parser_class.parser_file("test.pie");

  // cout << pr.first  << "\n";
  // cout << pr.second << "\n";

  return 0;
}