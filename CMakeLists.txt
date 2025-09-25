cmake_minimum_required(VERSION 3.10)

# Define project name (change this to rename the project)
set(PROJECT_NAME HelloProk)

# Project name and version
project(${PROJECT_NAME} VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set build output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Redirect compile_commands.json to CMakeFiles
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_COMMANDS_DIR ${CMAKE_BINARY_DIR}/CMakeFiles)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_COMPILE_COMMANDS_DIR}/compile_commands.json
  )
endif()

# Enable testing
enable_testing()

# Add executable for main project
add_executable(${PROJECT_NAME} src/main.cpp)

# Include directories for main project
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Add custom target for 'make run'
add_custom_target(run
  COMMAND ${PROJECT_NAME}
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "Running ${PROJECT_NAME}"
)

# Add test executable, explicitly using test folder
file(GLOB TEST_SOURCES test/*.cpp)
add_executable(${PROJECT_NAME}Tests ${TEST_SOURCES})

# Include directories for tests
target_include_directories(${PROJECT_NAME}Tests PRIVATE include)

# Find and link a testing framework (e.g., GoogleTest)
find_package(GTest REQUIRED)
target_link_libraries(${PROJECT_NAME}Tests GTest::GTest GTest::Main)

# Add test, output to build/test
add_test(
  NAME ${PROJECT_NAME}Tests
  COMMAND ${PROJECT_NAME}Tests
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build/test
)
