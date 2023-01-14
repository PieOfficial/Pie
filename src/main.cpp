#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "argparse.hpp"
#include "core/parser.h"
#include "script/carescript.hpp"

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
  //   std::cout << "Building" << std::endl;
  //   string line;
  //   ifstream myfile;
  //   myfile.open("build.pie");

  //  if(!myfile.is_open()) {
  //     perror("Error open");
  //     exit(EXIT_FAILURE);
  //  }
  //   while(getline(myfile, line)) {
  //    run_script(line);
  //   }
          std::string r;
        std::ifstream ifile("build.pie");
        while(ifile.good()) r += ifile.get();
        if(!r.empty()) r.pop_back();
        auto labels = into_labels(r);
        for(auto i : labels) {
            std::cout << i.first << " args: " << i.second.arglist.size() << " lines: " << i.second.lines.size() << "\n";
            std::cout << run_script(i.first,labels) << "\n";
        }
  }

  // parser_class parser_class;
  // parser_class.parser_file("test.pie");

  // cout << pr.first  << "\n";
  // cout << pr.second << "\n";

  return 0;
}