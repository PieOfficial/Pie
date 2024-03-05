/** C++ std library */
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>
#include "script/LuaManager.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#endif


//#include <sys/types.h>


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

    std::ifstream ifile(filename);
    std::string r(std::istreambuf_iterator<char>(ifile), {});

    // creates a new interpreter instance
    carescript::Interpreter interpreter;

    // save the current state as id 0
    interpreter.save(0);

    // This code runs when things go wrong, like a burnt pie
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

void read_pieScript_TIMEING(std::string filename, int numMeasurements) {
    long long totalElapsedTime = 0;

    for (int i = 0; i < numMeasurements; ++i) {
    // Start the timer
        high_resolution_clock::time_point start = high_resolution_clock::now();

        std::ifstream ifile(filename);
        std::string r(std::istreambuf_iterator<char>(ifile), {});

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
        totalElapsedTime += elapsed.count();
    }

    // Calculate and print the average time
    long long averageElapsedTime = totalElapsedTime / numMeasurements;
    std::cout << "Average CPU time used: " << averageElapsedTime << " microseconds\n";
}



int main(int argc, char *argv[]) {
    // 1. Create a Lua state
    //lua_State* L = luaL_newstate();

    argparse::ArgumentParser program("pie");

    program.add_argument("--build")
        .help("build root dir")
        .default_value(false) //std::string("build.pie")
        .implicit_value(true);
        //.nargs(1);
    program.add_argument("--testcurl")
        .help("test curl")
        .default_value(false)
        .implicit_value(true);
        //.nargs(1);
    program.add_argument("-d", "--download")
        .nargs(2)
        //.scan<'g', std::string>()
        //.default_value(std::string("none"))
        .help("Downloads a repo (repository) in the root dir");

    program.add_argument("--testlua")
        .action([&](const std::string &value)
        {
            lua_State* L = LuaManager::getInstance().getState();
            luaL_openlibs(L);  // Load standard Lua libraries
            luaL_loadstring(L, "print('Hello from Lua!')");
            int result = lua_pcall(L, 0, LUA_MULTRET, 0);
            if (result == LUA_OK) {
                lua_getglobal(L, "print");  // Get the Lua 'print' function
                lua_pushstring(L, "Hello from Lua!");
                lua_call(L, 1, 0);  // Call the 'print' function
            } else {
                std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
            }

            lua_close(L);
        })
        .default_value(std::string("none"))
        .help("lua");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }
    if (program.is_used("--download")) {
        auto files = program.get<std::vector<std::string>>("--download");
        const std::string &repo_url = files[0];
        const std::string &target_dir = files[1];
        std::cout << repo_url << std::endl;
        Network net;
        net.download_repo(repo_url, target_dir);
        //net.download_repo("repo_url", "target_dir");
    }
    if (program.is_used("--testcurl")) {
        Network net;
        net.testcurl();
    }
    if (program["--build"] == true) {
        read_pieScript("build.pie"); //program.get<std::string>("--build")
    }

    return 0;
}