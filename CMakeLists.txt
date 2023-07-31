cmake_minimum_required(VERSION 3.0.0)
get_filename_component(ProjectId ${CMAKE_SOURCE_DIR} NAME)
string(REPLACE " " "" ProjectId ${ProjectId})
project(${ProjectId} VERSION 0.1.0)

set(LINK_STATIC ON CACHE BOOL "Link libgcc and libstd statically?")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(LINK_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "template")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(CTest)
  add_subdirectory(test)
endif()