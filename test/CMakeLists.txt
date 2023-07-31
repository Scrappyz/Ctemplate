cmake_minimum_required(VERSION 3.12)

enable_testing()
add_subdirectory(googletest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(project_template_test ${CMAKE_CURRENT_SOURCE_DIR}/src/project_template_test.cpp)
target_include_directories(project_template_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(project_template_test PUBLIC gtest_main)

add_test(
    NAME ProjectTemplateTest
    COMMAND project_template_test
)
