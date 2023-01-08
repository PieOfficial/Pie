#include <iostream>
#include <string>

using namespace std;

// val is passed by value
pair<string, string> splitInTwo(string val) {
    string arg;
    string::size_type pos = val.find(':');
    if(val.npos != pos) {
        arg = val.substr(pos + 1);
        val = val.substr(0, pos);
    }
    return make_pair(val, arg);
}

int main() {
    string cmd = "HELLO:WORLD";

    pair<string, string> pr = splitInTwo(cmd);

    //cout << pr.first  << "\n";
    //cout << pr.second << "\n";

    return 0;
}