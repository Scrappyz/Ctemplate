cmake_minimum_required(VERSION 3.14)
get_filename_component(ProjectId ${CMAKE_SOURCE_DIR} NAME)
string(REPLACE " " "" ProjectId ${ProjectId})
project(${ProjectId} VERSION 0.1.0)

# Include modules
include(FetchContent)

set(LINK_STATIC ON CACHE BOOL "Link libgcc and libstd statically?")

# Set binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)

# List source files
file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add executable target
add_executable(${PROJECT_NAME} ${Sources})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(LINK_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()

# Unit testing
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  option(BUILT_TESTING "" OFF)
  include(CTest)
  add_subdirectory(test)
endif()