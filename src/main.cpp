#include <curl/curl.h>
#include <iostream>
#include <string>

// Callback function to handle data received from curl
size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *userp) {
  size_t total_size = size * nmemb;
  userp->append((char *)contents, total_size);
  return total_size;
}

// Function to fetch a URL and return the response
bool fetch_url(const std::string &url, std::string &response,
               std::string &error) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    error = "CURL initialization failed";
    return false;
  }

  response.clear();
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    error = curl_easy_strerror(res);
    curl_easy_cleanup(curl);
    return false;
  }

  long response_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  curl_easy_cleanup(curl);

  if (response_code != 200) {
    error = "HTTP status code: " + std::to_string(response_code);
    return false;
  }

  return true;
}

#ifndef UNIT_TEST
int main() {
  std::string response, error;
  if (fetch_url("https://www.google.com", response, error)) {
    std::cout << "Response from Google:\n" << response << std::endl;
  } else {
    std::cerr << "Error: " << error << std::endl;
    return 1;
  }
  return 0;
}
#endif
