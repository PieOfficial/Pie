#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem> // For file system operations

#include <curl/curl.h>

class Network {
public:


/**
 * Retrieves the contents of the specified URL using a GET request.
 *
 * @param url the URL to retrieve the contents from
 *
 * @return the response body as a string
 *
 * @throws std::string an error message if the request fails
 */
std::string get(const std::string& url);

std::string download_repo(const std::string& repo_url, const std::string& target_dir);

};

#endif
