#include <fstream>
#include <iostream>  // header in standard library
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "parser.h"  // header in local directory

using namespace parser;
using namespace std;

enum Tokens {
  var,
};

std::map<std::string, std::string> vars;

// Map to associate the strings with the enum values
static std::map<std::string, Tokens> s_mapTokensValues;

// static const enum tokens {
//     var = 'var',
// };

bool IsWhiteSpace(char c) {
  bool result = (c == ' ') || (c == '\t') || (c == '\f') || (c == '\v');
  return result;
}

bool IsEndOfLine(char c) {
  bool result = (c == '\n') || (c == '\r');
  return result;
}

bool IsLetter(char c) {
  bool result = false;

  if ((c >= 'A') && (c <= 'Z')) result = true;
  if ((c >= 'a') && (c <= 'z')) result = true;

  return result;
}

bool IsNumeric(char c) {
  bool result = false;

  if ((c >= '0') && (c <= '9')) result = true;

  return result;
}
// val is passed by value
pair<string, string> splitInTwo(string val) {
  string arg;
  string::size_type pos = val.find(':');
  if (val.npos != pos) {
    arg = val.substr(pos + 1);
    val = val.substr(0, pos);
  }
  return make_pair(val, arg);
}

std::string trim(const std::string& str) {
  auto first = str.find_first_not_of(' ');
  if (first == std::string::npos) first = 0;

  auto last = str.find_last_not_of(' ');
  if (last == std::string::npos) last = str.size();

  return str.substr(first, last - first + 1);
}

std::vector<std::string> ParseBrackets(std::string s) {
  std::vector<std::string> vector_name;
  std::regex rgx("\\{((?:\\s*\\w*\\s*\\,?)+)\\}");
  std::smatch match;

  if (std::regex_search(s, match, rgx) && match.size() == 2) {
    // match[1] now contains "sleep | work"
    std::istringstream iss(match[1]);
    std::string token;
    while (std::getline(iss, token, ',')) {
      // vector_name.insert (vector_name.end()+1, trim(token));
      vector_name.insert(vector_name.end(), trim(token));
      // std::cout << trim(token) << std::endl;
    }
    return vector_name;
  }
  return vector_name;
}

void removeCharsFromString(string& str, char* charsToRemove) {
  for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
    str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
  }
}

std::vector<std::string> SplitString(string line) {
  std::stringstream sstr(line);
  std::vector<std::string> v;
  while (sstr.good()) {
    std::string substr;
    getline(sstr, substr, ':');
    v.push_back(substr);
  }
  sstr.clear();
  return v;
}

std::vector<std::string> SplitStringComma(string line) {
  std::stringstream sstr(line);
  std::vector<std::string> v;
  while (sstr.good()) {
    std::string substr;
    getline(sstr, substr, ',');
    v.push_back(substr);
  }
  sstr.clear();
  return v;
}

// std::vector<std::string> v;

void parseLine(string line, string LineArgs) {
  // pair<string, string> pr = splitInTwo(line);
  // cout << line;
  // cout << pr.first  << "\n";
  std::vector<std::string> DoneLine = SplitString(line);
  // cout << DoneLine[0]  << "\n";

  if (DoneLine[0] == "var") {
    removeCharsFromString(DoneLine[1], "()");
    std::vector<std::string> args = SplitStringComma(DoneLine[1]);
    vars[args[0]] = args[1];

    // std::vector<std::string> v = ParseBrackets(DoneLine[1]);
    cout << DoneLine[1] << "\n";
    // cout << v[1]  << "\n";
    // cout << v[0]  << "\n";
  } else if (DoneLine[0] == "printvar") {
  }

  // switch (s_mapTokensValues[line])
  // {
  // case var:
  //     /* code */
  //     cout << LineArgs;
  //     v = ParseBrackets(LineArgs);
  //     cout << v[0];
  //     break;

  // default:
  //     break;
  // }
}

void Initialize() { s_mapTokensValues["var"] = var; }

void parser_class::parser_file(string path) {
  // Intialization
  // Initialize();

  // Read File
  string line;
  ifstream myfile;
  // string cmd = "HELLO:WORLD";

  myfile.open(path);

  if (!myfile.is_open()) {
    perror("Error open");
    exit(EXIT_FAILURE);
  }
  while (getline(myfile, line)) {
    // cout << line << endl;

    // cout << pr.first  << "\n";
    // cout << pr.second << "\n";

    parseLine(line, line);
  }
}

void parser_class::do_something() { cout << "Doing something!" << endl; }