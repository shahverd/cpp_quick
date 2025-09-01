#include <chrono>
#include <curl/curl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <vector>

// Include the functions we want to test from src/main.cpp
// Since your main.cpp is in src/, we'll declare them here
extern size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            std::string *userp);
extern bool fetch_url(const std::string &url, std::string &response,
                      std::string &error);

class CurlTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Initialize curl globally before tests
    curl_global_init(CURL_GLOBAL_DEFAULT);
  }

  void TearDown() override {
    // Cleanup curl after tests
    curl_global_cleanup();
  }
};

// Test the WriteCallback function
TEST_F(CurlTest, WriteCallbackBasicFunctionality) {
  std::string buffer;
  const char *test_data = "Hello, World!";
  size_t data_size = strlen(test_data);

  size_t result = WriteCallback((void *)test_data, 1, data_size, &buffer);

  EXPECT_EQ(result, data_size);
  EXPECT_EQ(buffer, "Hello, World!");
}

TEST_F(CurlTest, WriteCallbackMultipleCalls) {
  std::string buffer;
  const char *test_data1 = "Hello, ";
  const char *test_data2 = "World!";

  size_t result1 =
      WriteCallback((void *)test_data1, 1, strlen(test_data1), &buffer);
  size_t result2 =
      WriteCallback((void *)test_data2, 1, strlen(test_data2), &buffer);

  EXPECT_EQ(result1, strlen(test_data1));
  EXPECT_EQ(result2, strlen(test_data2));
  EXPECT_EQ(buffer, "Hello, World!");
}

TEST_F(CurlTest, WriteCallbackEmptyData) {
  std::string buffer;

  size_t result = WriteCallback(nullptr, 1, 0, &buffer);

  EXPECT_EQ(result, 0);
  EXPECT_TRUE(buffer.empty());
}

TEST_F(CurlTest, WriteCallbackLargeData) {
  std::string buffer;
  std::string large_data(10000, 'A'); // 10KB of 'A' characters

  size_t result =
      WriteCallback((void *)large_data.c_str(), 1, large_data.size(), &buffer);

  EXPECT_EQ(result, large_data.size());
  EXPECT_EQ(buffer, large_data);
}

// Test fetch_url function with real URLs
TEST_F(CurlTest, FetchUrlGoogleSuccess) {
  std::string response, error;

  bool result = fetch_url("https://www.google.com", response, error);

  EXPECT_TRUE(result);
  EXPECT_FALSE(response.empty());
  EXPECT_TRUE(error.empty());
  // Check that we got some HTML back
  EXPECT_THAT(response, ::testing::HasSubstr("html"));
}

TEST_F(CurlTest, FetchUrlHttpBinSuccess) {
  std::string response, error;

  bool result = fetch_url("https://httpbin.org/get", response, error);

  EXPECT_TRUE(result);
  EXPECT_FALSE(response.empty());
  EXPECT_TRUE(error.empty());
  // httpbin.org/get returns JSON
  EXPECT_THAT(response, ::testing::HasSubstr("origin"));
}

TEST_F(CurlTest, FetchUrlInvalidDomain) {
  std::string response, error;

  bool result =
      fetch_url("https://this-domain-definitely-does-not-exist-12345.com",
                response, error);

  EXPECT_FALSE(result);
  EXPECT_FALSE(error.empty());
  EXPECT_TRUE(response.empty());
}

TEST_F(CurlTest, FetchUrlInvalidProtocol) {
  std::string response, error;

  bool result = fetch_url("invalid://www.google.com", response, error);

  EXPECT_FALSE(result);
  EXPECT_FALSE(error.empty());
}

TEST_F(CurlTest, FetchUrl404Error) {
  std::string response, error;

  bool result = fetch_url("https://httpbin.org/status/404", response, error);

  EXPECT_FALSE(result);
  EXPECT_FALSE(error.empty());
  EXPECT_THAT(error, ::testing::HasSubstr("404"));
}

TEST_F(CurlTest, FetchUrl500Error) {
  std::string response, error;

  bool result = fetch_url("https://httpbin.org/status/500", response, error);

  EXPECT_FALSE(result);
  EXPECT_FALSE(error.empty());
  EXPECT_THAT(error, ::testing::HasSubstr("500"));
}

TEST_F(CurlTest, FetchUrlRedirectHandling) {
  std::string response, error;

  // httpbin.org/redirect/1 will redirect once
  bool result = fetch_url("https://httpbin.org/redirect/1", response, error);

  EXPECT_TRUE(result);
  EXPECT_FALSE(response.empty());
  EXPECT_TRUE(error.empty());
}

TEST_F(CurlTest, FetchUrlEmptyUrl) {
  std::string response, error;

  bool result = fetch_url("", response, error);

  EXPECT_FALSE(result);
  EXPECT_FALSE(error.empty());
}

TEST_F(CurlTest, FetchUrlMalformedUrl) {
  std::string response, error;

  bool result = fetch_url("not-a-url", response, error);

  EXPECT_FALSE(result);
  EXPECT_FALSE(error.empty());
}

// Performance test - should complete within reasonable time
TEST_F(CurlTest, FetchUrlPerformance) {
  std::string response, error;

  auto start = std::chrono::high_resolution_clock::now();
  bool result = fetch_url("https://httpbin.org/get", response, error);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

  EXPECT_TRUE(result);
  EXPECT_LT(duration.count(), 30); // Should complete within 30 seconds
}

// Test concurrent requests (if needed)
TEST_F(CurlTest, FetchUrlConcurrent) {
  std::vector<std::thread> threads;
  std::vector<bool> results(3);
  std::vector<std::string> responses(3);
  std::vector<std::string> errors(3);

  for (int i = 0; i < 3; ++i) {
    threads.emplace_back([&, i]() {
      results[i] =
          fetch_url("https://httpbin.org/get", responses[i], errors[i]);
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(results[i]) << "Request " << i << " failed: " << errors[i];
    EXPECT_FALSE(responses[i].empty()) << "Response " << i << " is empty";
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
