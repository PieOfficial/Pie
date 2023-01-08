#include "parser.h" // header in local directory
#include <iostream> // header in standard library
#include <fstream>
#include <string>

using namespace N;
using namespace std;

void parser_class::parser_file(string path) {
    string line;
    ifstream myfile;
    //string cmd = "HELLO:WORLD";

    
    myfile.open(path);

   if(!myfile.is_open()) {
      perror("Error open");
      exit(EXIT_FAILURE);
   }
    while(getline(myfile, line)) {
     cout << line << endl;
     pair<string, string> pr = splitInTwo(line);
     cout << pr.first  << "\n";
     cout << pr.second << "\n";
    }
}


void parser_class::do_something()
{
    cout << "Doing something!" << endl;
}