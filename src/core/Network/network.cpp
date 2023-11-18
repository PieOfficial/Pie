#include "network.hpp"

namespace Network {

static size_t WriteCallback(void *buffer, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)buffer, size * nmemb);
  return size * nmemb;
}

std::string get(std::string url) {
  CURL *curl = curl_easy_init();
  if (curl) {
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
      return response;
    } else {
      std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
    }
  } else {
    std::cerr << "Failed to initialize curl" << std::endl;
  }

  throw std::string("Failed to retrieve contents from URL: " + url);
}

} // namespace Network
