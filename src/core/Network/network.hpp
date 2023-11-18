#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <string>
#include "../../../include/curl/curl/curl.h"

namespace Network {

static size_t WriteCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)buffer, size * nmemb);
  return size * nmemb;
}

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
  }

  return 0;
}

}
#endif