#include "gtest/gtest.h"
#include "main.hpp"
#include "path.hpp"

std::string test_path = path::joinPath(path::sourcePath(), "../test_path");
std::string config_path = path::joinPath(test_path, "program/config.txt");

TEST(some_test, some_test)
{
    
}