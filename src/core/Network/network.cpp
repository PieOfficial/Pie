#include "network.hpp"
#include <stdio.h>

namespace fs = std::filesystem;

/**
 * Writes data to a file stream.
 *
 * @param ptr A pointer to the data to be written.
 * @param size The size of each element to be written.
 * @param nmemb The number of elements to be written.
 * @param stream A pointer to the file stream.
 *
 * @return The total number of bytes written.
 *
 * @throws None.
 */
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::ofstream *file = (std::ofstream *)stream;
    file->write((char *)ptr, size * nmemb);
    return size * nmemb;
}


std::string Network::download_repo(const std::string& repo_url, const std::string& target_dir) {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    std::string file_name = "";

    // Create a curl handle
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    if (curl) {
        // Set URL to download
        curl_easy_setopt(curl, CURLOPT_URL, repo_url.c_str());

        // Optimize for speed
        curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);  // Disable Nagle's algorithm
        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1); // Force a new connection
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);  // Disable connection reuse
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // Follow redirects
        //curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1000L);  // Set minimum transfer speed
        //curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5L);      // Abort after 5 seconds below speed limit

        // Set callback function for writing data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        // Create the target directory if it doesn't exist
        std::filesystem::create_directories(target_dir);

        // Open a file to store the downloaded data
        std::ofstream file("downloaded_file.zip", std::ios::binary);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

        // Perform the download
        CURLcode res = curl_easy_perform(curl);

        // Close the file
        file.close();

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Move the downloaded file to the target directory
        try {
            std::filesystem::copy_file("downloaded_file.zip", target_dir + "/downloaded_file.zip");
        } catch (std::filesystem::filesystem_error& err) {
          std::cerr << "Error moving file: " << err.what() << std::endl;
        }
        file_name = "downloaded_file.zip";
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return file_name;
}



std::string Network::get(const std::string& url) {
  return "";
}