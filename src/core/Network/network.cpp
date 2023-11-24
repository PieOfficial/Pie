#include "network.hpp"
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

#pragma comment(lib, "winhttp.lib")
//#include <curl/curl.h> //"../../../include/curl/curl/curl.h"

std::size_t Network::WriteCallback(void *buffer, std::size_t size, std::size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)buffer, size * nmemb);
  return size * nmemb;
}

std::string  Network::get(const std::string& url) {
  HINTERNET hSession = WinHttpOpen(L"MyUserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_DEFAULT_LOOPBACK_ADDRESS, WINHTTP_FLAG_READ_FROM_CACHE);
  if (!hSession) {
    printf("WinHttpOpen failed: %d\n", GetLastError());
    return "";
  }

  HINTERNET hConnect = WinHttpConnect(hSession, url.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
  if (!hConnect) {
    printf("WinHttpConnect failed: %d\n", GetLastError());
    WinHttpCloseHandle(hSession);
    return "";
  }

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/", L"HTTP/1.1", L"Host: www.example.com", WINHTTP_NO_ADDITIONAL_HEADERS, WINHTTP_NO_FLAGS, 0);
  if (!hRequest) {
    printf("WinHttpOpenRequest failed: %d\n", GetLastError());
    WinHttpCloseHandle(hSession);
    WinHttpCloseHandle(hConnect);
    return "";
  }

  BOOL bSuccess = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, 0);
  if (!bSuccess) {
    printf("WinHttpSendRequest failed: %d\n", GetLastError());
    WinHttpCloseHandle(hSession);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hRequest);
    return "";
  }

  DWORD dwNumberOfBytesRead = 0;
  char buffer[1024];
  std::string response;
  do {
    bSuccess = WinHttpReadData(hRequest, buffer, sizeof(buffer), &dwNumberOfBytesRead);
    if (!bSuccess) {
      if (GetLastError() != ERROR_SUCCESS) {
        printf("WinHttpReadData failed: %d\n", GetLastError());
      }
      break;
    }

    response.append(buffer, dwNumberOfBytesRead);
  } while (dwNumberOfBytesRead > 0);

  WinHttpCloseHandle(hSession);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hRequest);

  return response;
  // CURL *curl = curl_easy_init();
  // if (curl) {
  //   std::string response;

  //   curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  //   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Network::WriteCallback);
  //   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  //   CURLcode res = curl_easy_perform(curl);
  //   curl_easy_cleanup(curl);

  //   if (res == CURLE_OK) {
  //     return response;
  //   } else {
  //     std::cerr << "curl error: " << curl_easy_strerror(res) << std::endl;
  //   }
  // } else {
  //   std::cerr << "Failed to initialize curl" << std::endl;
  // }

  // throw std::string("Failed to retrieve contents from URL: " + url);
}


