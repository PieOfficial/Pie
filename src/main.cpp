/* 
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 */
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


#include "argparse.hpp"
#include "core/parser.h"
#include "core/framework.hpp"
#include "script/carescript.hpp"

using namespace parser;
using namespace framework;
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
    //   std::cout << "Building" << std::endl;
    std::clock_t c_start = std::clock();  // Track Time Taken
    // Start read file
    std::string r;
    std::ifstream ifile("build.pie");
    while (ifile.good()) r += ifile.get();
    if (!r.empty()) r.pop_back();
    auto labels = into_labels(r);
    for (auto i : labels) {
      std::cout << i.first << " args: " << i.second.arglist.size()
                << " lines: " << i.second.lines.size() << "\n";
      std::cout << run_script(i.first, labels) << "\n";
    }
    std::clock_t c_end = std::clock();

    double time_elapsed_ms = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
    std::cout << "CPU time used: " << time_elapsed_ms << " ms\n";
  }

  // parser_class parser_class;
  // parser_class.parser_file("test.pie");

  return 0;
}