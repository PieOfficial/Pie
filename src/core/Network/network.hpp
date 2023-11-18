#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <string>
#include "../../../include/curl/curl/curl.h"

namespace Network {

/**
 * Callback function used by the libcurl library to write data into a string buffer.
 *
 * @param buffer A pointer to the buffer where the data should be written.
 * @param size The size of each element to be written.
 * @param nmemb The number of elements to be written.
 * @param userp A pointer to the user-defined data.
 *
 * @return The total number of bytes written into the buffer.
 */
static size_t WriteCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)buffer, size * nmemb);
  return size * nmemb;
}

/**
 * Retrieves the contents of the specified URL using a GET request.
 *
 * @param url the URL to retrieve the contents from
 *
 * @return the response body as a string
 *
 * @throws std::string an error message if the request fails
 */
std::string Network::get(std::string url) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
      //std::cout << response << std::endl;
      return response;
    } else {
      std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
    }
  } else {
    std::cerr << "Failed to initialize curl" << std::endl;
  }

}

}
#endif