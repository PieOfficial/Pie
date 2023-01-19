#ifndef GIT_CLASS_H // include guard
#define GIT_CLASS_H

#include <string>

#include "git/restless.hpp"
#include <curl/curl.h>

namespace git
{
    class git_class
    {
    public:
        void download_repo(std::string username, std::string repo, std::string branch);
       
    };
}

#endif 