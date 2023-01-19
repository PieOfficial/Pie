/* 
https://codeload.github.com/LabRicecat/CatCaretaker/zip/main

 */
#include <iostream> // header in standard library
#include "git.h" // header in local directory


using namespace git;
using namespace std;


void git_class::download_repo(std::string username, std::string repo, std::string branch) {
    std::string url = "https://codeload.github.com/" + username + "/" + repo + "/zip/" + branch;
    asoni::Handle().get(url).exec();
}