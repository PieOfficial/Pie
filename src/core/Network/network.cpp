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


/**
 * Test the libcurl library by sending a request to a publicly accessible server.
 * @return 0 if the test is successful, 1 if there is an error.
 */
int Network::testcurl() {
  // Initialize curl
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Error: curl_easy_init() failed." << std::endl;
    return 1;
  }

  // Set URL to a publicly accessible server that doesn't require authentication
  curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com"); // Replace with a valid URL

  // Set options to avoid downloading data
  curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Request headers only
  curl_easy_setopt(curl, CURLOPT_HEADER, 0L); // Suppress header output

  // Perform the request
  CURLcode res = curl_easy_perform(curl);

  // Check for errors
  if (res != CURLE_OK) {
    std::cerr << "Error: curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    return 1;
  }

  // Check response code
  long http_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  if (http_code != 200) {
    std::cerr << "Error: Received HTTP code " << http_code << " (expected 200)." << std::endl;
    return 1;
  }

  // Success!
  std::cout << "libcurl seems to be working correctly." << std::endl;

  // Cleanup
  curl_easy_cleanup(curl);
  return 0;
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
        std::ofstream file(target_dir + "/downloaded_file.zip", std::ios::binary);

        if (file.is_open()) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

            // Perform the download
            CURLcode res = curl_easy_perform(curl);

            // Close the file
            file.close();

            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            } else {
                std::cout << "File downloaded successfully." << std::endl;
                file_name = target_dir + "/downloaded_file.zip";
            }
        } else {
            std::cerr << "Error opening file for writing: " << target_dir + "/downloaded_file.zip" << std::endl;
        }
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Error initializing curl handle." << std::endl;
    }

    // Clean up libcurl
    curl_global_cleanup();

    return file_name;
}



std::string Network::get(const std::string& url) {
  return "";
}