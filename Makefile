# Compiler and flags
PROJECT_NAME = CurlGoog
CXX = clang++
CXXFLAGS = -g -std=c++23 -Wall -Wextra
LDFLAGS = $(shell pkg-config --libs libcurl)
CFLAGS = $(shell pkg-config --cflags libcurl)
# Google Test flags
GTEST_FLAGS = $(shell pkg-config --cflags --libs gtest gtest_main gmock)
TEST_CFLAGS = $(CFLAGS) -I$(CURDIR) -DUNIT_TEST
# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
BIN_DIR = bin
# Main program
#MAIN_TARGET = $(BIN_DIR)/$(PROJECT_NAME)
MAIN_TARGET = $(BIN_DIR)/main
MAIN_SOURCE = $(SRC_DIR)/main.cpp
# Test program
TEST_TARGET = $(BIN_DIR)/test_main
TEST_SOURCE = $(TEST_DIR)/test_main.cpp
# Object files
MAIN_OBJECT = $(BUILD_DIR)/main.o
TEST_OBJECT = $(BUILD_DIR)/test_main.o
# Default target
all: $(MAIN_TARGET)
# Run target - builds and executes the main program
run: $(MAIN_TARGET)
	$(MAIN_TARGET)
# Test target
test: $(TEST_TARGET)
	$(TEST_TARGET)
# Main program compilation
$(MAIN_TARGET): $(MAIN_OBJECT) | $(BIN_DIR)
	$(CXX) $(MAIN_OBJECT) -o $(MAIN_TARGET) $(LDFLAGS)
# Test program compilation
$(TEST_TARGET): $(TEST_OBJECT) $(BUILD_DIR)/main_for_test.o | $(BIN_DIR)
	$(CXX) $(TEST_OBJECT) $(BUILD_DIR)/main_for_test.o -o $(TEST_TARGET) $(LDFLAGS) $(GTEST_FLAGS)
# Compile main source
$(MAIN_OBJECT): $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@
# Compile main source for testing (with UNIT_TEST defined)
$(BUILD_DIR)/main_for_test.o: $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_CFLAGS) -c $< -o $@
# Compile test source
$(TEST_OBJECT): $(TEST_SOURCE) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_CFLAGS) $(GTEST_FLAGS) -c $< -o $@
# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(TEST_DIR):
	mkdir -p $(TEST_DIR)
# Clean up
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
# Rebuild
rebuild: clean all
# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libcurl4-openssl-dev libgtest-dev libgmock-dev pkg-config
# Install dependencies (macOS with Homebrew)
install-deps-mac:
	brew install curl googletest pkg-config
# Run tests with verbose output
test-verbose: $(TEST_TARGET)
	$(TEST_TARGET) --gtest_verbose
# Run specific test
test-filter: $(TEST_TARGET)
	$(TEST_TARGET) --gtest_filter="*$(FILTER)*"
# Generate test coverage (requires gcov)
coverage: CXXFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: test
	gcov $(BUILD_DIR)/*.gcno
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report
# Phony targets
.PHONY: all run test clean rebuild install-deps install-deps-mac test-verbose test-filter coverage
