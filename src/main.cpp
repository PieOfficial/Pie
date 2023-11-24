/** C++ std library */
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>


#ifdef _WIN32
#include <windows.h>
//#include "unistd.h"
//#include <process.h>
#elif __linux__
#include <unistd.h>
#endif

// #include <sys/wait.h>
#include <sys/types.h>


#include "../include/argparse.hpp"

#include "script/carescript-api.hpp"
#include "core/console/colored_cout.h"
#include "core/Network/network.hpp"

using namespace std;
using namespace chrono;

using std::cerr;
using std::cout;
using std::endl;
using std::getline;  /** reads a line from a file and puts it in a string */
using std::ifstream; /** class representing a file to read */
using std::ostream;

void read_pieScript(std::string filename) {
    // Start the timer
    high_resolution_clock::time_point start = high_resolution_clock::now();

    std::string r;
    std::ifstream ifile(filename);
    while (ifile.good())
        r += ifile.get();
    if (r != "")
        r.pop_back();

    // creates a new interpreter instance
    carescript::Interpreter interpreter;

    // save the current state as id 0
    interpreter.save(0);

    // This code executes when an error occurs
    interpreter.on_error([](carescript::Interpreter &interp) {
        // #ifdef _WIN32
        // // Beep at 200 Hz for 500 milliseconds.
        // Beep(200, 500);

        // // Beep at 400 Hz for 500 milliseconds.
        // Beep(400, 500);

        // // Beep at 600 Hz for 500 milliseconds.
        // Beep(600, 500);
        // // Define an array of frequencies to use.
        // int frequencies[] = {500, 1000, 1500, 2000};

        // // Loop through the frequencies and play a beep sound at each one.
        // for (int i = 0; i < sizeof(frequencies) / sizeof(frequencies[0]); i++) {
        //     Beep(frequencies[i], 100); // Beep for 100 milliseconds.
        // }
        // #endif
        std::cout << RED << interp.error() << NC << std::endl; 
    });

    // pre processes the code
    interpreter.pre_process(r);

    // runs the "main" label
    interpreter.run();
    // // runs the label "some_label" with the arguments 1, 2 and 3
    // interpreter.run("some_label",1,2,3);

    // // runs the label "label_with_return" and unwraps the return value
    // carescript::ScriptVariable value = interpreter.run("label_with_return").get_value();

    interpreter.load(0); // loads the saved state with id 0

    // Stop the timer
    high_resolution_clock::time_point end = high_resolution_clock::now();

    // Calculate the time elapsed
    duration<double, milli> elapsed = end - start;

    // Subtract the overhead of the benchmark itself
    duration<double, milli> overhead = duration_cast<milliseconds>(
        high_resolution_clock::now() - high_resolution_clock::now());
    elapsed -= overhead;

    // Print the results
    std::cout << "CPU time used: " << elapsed.count() << " ms\n";
}

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("pie");

    program.add_argument("--build")
        .help("build root dir")
        .default_value(false) //std::string("build.pie")
        .implicit_value(true);
        //.nargs(1);

    program.add_argument("-d", "--download")
        .action([&](const std::string &value)
        {
            std::cout << value << std::endl;
            Network net;
            std::cout << net.get(value) << std::endl;
        })
        .default_value(std::string("none"))
        .help("Downloads a repo (repository) in the root dir");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    if (program["--build"] == true) {
        read_pieScript("build.pie"); //program.get<std::string>("--build")
    }

    return 0;
}