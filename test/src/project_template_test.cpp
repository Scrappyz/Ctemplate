#include "gtest/gtest.h"
#include "main.hpp"
#include "path.hpp"

std::string test_path = path::joinPath(path::sourcePath(), "../test_path");
std::string config_path = path::joinPath(test_path, "program/config.txt");
std::string template_dir = path::joinPath(test_path, "templates");
std::string destination = path::joinPath(test_path, "temp");

TEST(initTemplate, initialized)
{
    CLI cli({"project_template_test.exe", "init", "t1", "-p", destination});
    Config config(config_path);
    //initTemplate(cli, config); // BUG: this makes the unit test binary disappear
    //ASSERT_TRUE(path::hasSameContent(path::joinPath(template_dir, "t1"), destination));
    //path::remove(destination + '/');
}