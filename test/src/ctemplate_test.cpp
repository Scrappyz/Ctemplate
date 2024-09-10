#include "gtest/gtest.h"
#include "ctemplate.hpp"
#include "helper.hpp"
#include "os.hpp"

namespace path = os::path;
using json = nlohmann::json;

std::string test_path = path::joinPath(path::sourcePath(), "../test_path");
std::string template_path = path::joinPath(test_path, "templates");
std::string temp_path = path::joinPath(test_path, "temp");
std::string container_name = ".ctemplate";
std::string cache_container_name = ".cache";

std::unordered_set<std::string> normalizePaths(const std::unordered_set<std::string>& s, const std::string& relative_to = "")
{
    std::unordered_set<std::string> n;
    for(const auto& i : s) {
        std::string p = path::relativePath(path::joinPath(relative_to, i), relative_to);
        n.insert(p);
    }

    return n;
}

std::set<std::string> normalizePaths(const std::set<std::string>& s, const std::string& relative_to = "")
{
    std::set<std::string> n;
    for(const auto& i : s) {
        std::string p = path::relativePath(path::joinPath(relative_to, i), relative_to);
        n.insert(p);
    }

    return n;
}

TEST(resetConfig, app_config)
{
    std::string config_file = path::joinPath(path::sourcePath(), "config.json");
    json temp = helper::readJsonFromFile(config_file);
    json expected = {
        {"templateDirectory", path::joinPath(path::sourcePath(), "templates")},
        {"containerName", container_name}
    };

    helper::resetConfig();

    EXPECT_EQ(expected, helper::readJsonFromFile(config_file));

    helper::writeJsonToFile(temp, config_file, 4);
}

TEST(resetConfig, template_config)
{
    std::string suite_path = path::joinPath(test_path, "test_suites/reset_config");
    std::string suite_template_path = path::joinPath(suite_path, "templates");

    std::vector<std::string> templates = {"py"};
    
    std::string container_path = path::joinPath(suite_template_path, "py/.ctemplate");
    ASSERT_FALSE(path::exists(container_path));

    helper::resetConfig(suite_template_path, container_name, templates);

    path::remove(container_path);
}

TEST(initTemplate, working_on_empty_dir)
{
    std::string test_template_path = path::joinPath(test_path, "test_suites/init_template/test_templates/py");
    std::string t_path = path::joinPath(test_path, "test_suites/init_template/test");
    std::string tmp_path = path::joinPath(test_path, "test_suites/init_template/temp");
    std::unordered_map<std::string, std::string> keyval = {{"project", "hello_world"}, {"name", "User"}};
    std::string t_file = path::joinPath(t_path, "junk.txt");
    ASSERT_TRUE(path::createFile(t_file, "test data"));

    ASSERT_TRUE(path::exists(t_file));

    initTemplate(test_template_path, container_name, t_path, keyval);

    ASSERT_TRUE(path::exists(t_file));

    path::remove(t_path + path::directorySeparator());
}

TEST(initTemplate, overwrite_all)
{
    std::string test_template_path = path::joinPath(test_path, "test_suites/init_template/test_templates/py");
    std::string t_path = path::joinPath(test_path, "test_suites/init_template/test");
    std::string tmp_path = path::joinPath(test_path, "test_suites/init_template/temp");
    std::unordered_map<std::string, std::string> keyval = {{"project", "hello_world"}, {"name", "User"}};
    std::string t_file = path::joinPath(t_path, "test.txt");
    path::createFile(t_file, "test data");

    ASSERT_TRUE(path::exists(t_file));
     
    initTemplate(test_template_path, container_name, t_path, keyval, true);

    ASSERT_TRUE(!path::exists(t_file));

    ASSERT_TRUE(path::exists(path::joinPath(t_path, "hello_world")));
    ASSERT_TRUE(path::exists(path::joinPath(t_path, "hello_world/hello_world.py")));

    ASSERT_TRUE(!path::exists(path::joinPath(t_path, container_name)));

    std::string actual_file_content = helper::readTextFromFile(path::joinPath(t_path, "hello_world/hello_world.py"));
    std::string expected_file_content = helper::readTextFromFile(path::joinPath(tmp_path, "test.py"));

    EXPECT_EQ(actual_file_content, expected_file_content);

    path::remove(t_path + path::directorySeparator());
}

TEST(initTemplate, overwrite_all_cancel)
{
    std::string test_template_path = path::joinPath(test_path, "test_suites/init_template/test_templates/py");
    std::string t_path = path::joinPath(test_path, "test_suites/init_template/test");
    std::string tmp_path = path::joinPath(test_path, "test_suites/init_template/temp");
    std::unordered_map<std::string, std::string> keyval = {{"project", "hello_world"}, {"name", "User"}};
    std::string t_file = path::joinPath(t_path, "test.txt");
    path::createFile(t_file, "test data");

    ASSERT_TRUE(path::exists(t_file));
     
    initTemplate(test_template_path, container_name, t_path, keyval);

    ASSERT_TRUE(path::exists(t_file));

    path::remove(t_path + path::directorySeparator());
}

TEST(initTemplate, custom_includes)
{
    std::string test_template_path = path::joinPath(test_path, "test_suites/init_template/test_templates/py");
    std::string t_path = path::joinPath(test_path, "test_suites/init_template/test");
    std::string tmp_path = path::joinPath(test_path, "test_suites/init_template/temp");
    std::unordered_map<std::string, std::string> keyval = {{"project", "hello_world"}, {"name", "User"}};
    std::set<std::string> paths = helper::matchPaths(helper::getPaths(test_template_path, test_template_path), {"!project!", "!project!/**"}, {});
     
    initTemplate(test_template_path, paths, container_name, t_path, keyval);

    ASSERT_TRUE(path::exists(path::joinPath(t_path, "hello_world")));
    ASSERT_TRUE(path::exists(path::joinPath(t_path, "hello_world/hello_world.py")));

    ASSERT_TRUE(!path::exists(path::joinPath(t_path, container_name)));
    ASSERT_TRUE(!path::exists(path::joinPath(t_path, "test")));

    std::string actual_file_content = helper::readTextFromFile(path::joinPath(t_path, "hello_world/hello_world.py"));
    std::string expected_file_content = helper::readTextFromFile(path::joinPath(tmp_path, "test.py"));

    EXPECT_EQ(actual_file_content, expected_file_content);

    path::remove(t_path + path::directorySeparator());
}

TEST(initTemplate, strange_case)
{
    std::string test_template_path = path::joinPath(test_path, "test_suites/init_template/test_templates/py");
    std::string t_path = path::joinPath(test_path, "test_suites/init_template/test");
    std::string tmp_path = path::joinPath(test_path, "test_suites/init_template/temp");
    std::unordered_map<std::string, std::string> keyval = {{"project", "project_world"}, {"name", "User"}};
    std::string t_file = path::joinPath(t_path, "test.txt");
    path::createFile(t_file, "test data");

    ASSERT_TRUE(path::exists(t_file));
     
    initTemplate(test_template_path, container_name, t_path, keyval, true);

    ASSERT_TRUE(!path::exists(t_file));

    ASSERT_TRUE(path::exists(path::joinPath(t_path, "project_world")));
    ASSERT_TRUE(path::exists(path::joinPath(t_path, "project_world/project_world.py")));

    ASSERT_TRUE(!path::exists(path::joinPath(t_path, container_name)));

    std::string actual_file_content = helper::readTextFromFile(path::joinPath(t_path, "project_world/project_world.py"));
    std::string expected_file_content = helper::readTextFromFile(path::joinPath(tmp_path, "test.py"));

    EXPECT_EQ(actual_file_content, expected_file_content);

    path::remove(t_path + path::directorySeparator());
}

TEST(initTemplate, cache_generate)
{
    std::string test_template_path = path::joinPath(test_path, "test_suites/init_template/test_templates/py");
    std::string t_path = path::joinPath(test_path, "test_suites/init_template/test");
    std::string tmp_path = path::joinPath(test_path, "test_suites/init_template/temp");
    std::unordered_map<std::string, std::string> keyval = {{"project", "project_world"}, {"name", "User"}};
    std::string cache_path = path::joinPath(test_template_path, ".ctemplate/.cache");
     
    initTemplate(test_template_path, container_name, t_path, keyval, true);

    ASSERT_TRUE(path::exists(cache_path));

    path::remove(t_path + path::directorySeparator());
}

TEST(initTemplate, cache_rewrite)
{
    std::string test_template_path = path::joinPath(test_path, "test_suites/init_template/test_templates/py");
    std::string t_path = path::joinPath(test_path, "test_suites/init_template/test");
    std::string tmp_path = path::joinPath(test_path, "test_suites/init_template/temp");
    std::unordered_map<std::string, std::string> keyval = {{"project", "project_world"}, {"name", "User"}};
    std::string cache_path = path::joinPath(test_template_path, ".ctemplate/.cache");
    std::string vars_path = path::joinPath(cache_path, "../variables.json");
    std::string paths_cache_path = path::joinPath(cache_path, "included_search_paths.json");

    ASSERT_TRUE(path::exists(cache_path));

    json vars = helper::readJsonFromFile(vars_path);
    json paths_cache = helper::readJsonFromFile(paths_cache_path);

    EXPECT_EQ(paths_cache.at("files")[0], path::normalizePath(vars.at("searchPaths").at("files").at("include")[0]));

    vars.at("searchPaths").at("files")["include"] = {};
    helper::writeJsonToFile(vars, vars_path, 4);
     
    initTemplate(test_template_path, container_name, t_path, keyval, true);

    paths_cache = helper::readJsonFromFile(paths_cache_path);

    EXPECT_EQ(paths_cache.at("files"), json::parse("[]"));

    vars.at("searchPaths").at("files")["include"] = {"!project!/!project!.py"};
    helper::writeJsonToFile(vars, vars_path, 4);

    path::remove(t_path + path::directorySeparator());
    path::remove(cache_path);
}

TEST(addTemplate, adding)
{
    std::string add_path = path::joinPath(template_path, "t1");
    std::string name = "test_suites";
    std::string author = "scrap";
    std::string desc = "wassup boi";
    addTemplate(template_path, add_path, name, author, desc, container_name);

    std::string new_template = path::joinPath(template_path, "test_suites");
    ASSERT_TRUE(path::exists(new_template));
    ASSERT_TRUE(path::exists(path::joinPath(new_template, container_name)));
    
    path::remove(new_template);
}

TEST(addTemplate, empty_name)
{
    std::string add_path = path::joinPath(template_path, "t1");
    std::string name = "test_suites";
    std::string author = "scrap";
    std::string desc = "wassup boi";
    addTemplate(template_path, add_path, "", author, desc, container_name);

    ASSERT_TRUE(!path::exists(name));
}

TEST(addTemplate, existing_container)
{
    std::string add_path = path::joinPath(template_path, "t1");
    std::string name = "test_suites";
    std::string author = "scrap";
    std::string desc = "wassup boi";
    addTemplate(template_path, add_path, name, author, desc, container_name);

    std::string new_template = path::joinPath(template_path, "test_suites");
    std::string new_container = path::joinPath(new_template, container_name);
    ASSERT_TRUE(path::exists(path::joinPath(new_container, "info.json")));
    ASSERT_TRUE(path::exists(path::joinPath(new_container, "variables.json")));

    path::remove(new_template);
}

TEST(removeTemplates, removing)
{
    path::copy(path::joinPath(template_path, "t1"), temp_path);
    path::copy(path::joinPath(template_path, "t2"), temp_path);

    ASSERT_TRUE(path::exists(path::joinPath(template_path, "t1")));
    ASSERT_TRUE(path::exists(path::joinPath(template_path, "t2")));
    removeTemplates(template_path, {"t1", "t2"});
    ASSERT_TRUE(!path::exists(path::joinPath(template_path, "t1")));
    ASSERT_TRUE(!path::exists(path::joinPath(template_path, "t2")));

    path::copy(path::joinPath(temp_path, "t1"), template_path, path::CopyOption::OverwriteExisting);
    path::copy(path::joinPath(temp_path, "t2"), template_path, path::CopyOption::OverwriteExisting);

    path::remove(path::joinPath(temp_path, "t1"));
    path::remove(path::joinPath(temp_path, "t2"));
}

TEST(split, splitting)
{
    std::vector<std::string> s = helper::split("key=value", "=");
    EXPECT_EQ(s, std::vector<std::string>({"key", "value"}));
}

TEST(mapKeyValues, mapping)
{
    std::vector<std::string> s = {"project=hello", "name=johnny sins", "order=asc", "something"};
    std::unordered_map<std::string, std::string> m = helper::mapKeyValues(s);
    std::unordered_map<std::string, std::string> expected_m = {{"project", "hello"}, {"name", "johnny sins"}, {"order", "asc"}};
    EXPECT_EQ(m, expected_m);
}

TEST(replaceVariables, replacing)
{
    std::string str = "My name is !name! and I am !age! years old.";
    std::string prefix = "!";
    std::string suffix = "!";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and I am 12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, longer_prefix_and_suffix)
{
    std::string str = "My name is {!name!} and I am {!age!} years old.";
    std::string prefix = "{!";
    std::string suffix = "!}";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and I am 12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, uneven_prefix_and_suffix)
{
    std::string str = "My name is {!name!}] and I am {!age!}] years old.";
    std::string prefix = "{!";
    std::string suffix = "!}]";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and I am 12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, missing_prefix)
{
    std::string str = "My name is name!}] and I am age!}] years old.";
    std::string prefix = "{!";
    std::string suffix = "!}]";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is name!}] and I am age!}] years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, missing_suffix)
{
    std::string str = "My name is {!name and I am {!age years old.";
    std::string prefix = "{!";
    std::string suffix = "!}]";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is {!name and I am {!age years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, missing_variables)
{
    std::string str = "My name is !name! and";
    std::string prefix = "!";
    std::string suffix = "!";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, endlines_in_str)
{
    std::string str = "My name is {!name!}\nand I am\n{!age!} years old.";
    std::string prefix = "{!";
    std::string suffix = "!}";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John\nand I am\n12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, endlines_in_var)
{
    std::string str = "My name is {!name!} and I am {!age!} years old.";
    std::string prefix = "{!";
    std::string suffix = "!}";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John\nWick"}, {"age", "12\nteen"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John\nWick and I am 12\nteen years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, edge_cases)
{
    std::string str = "My name is !name! and I am !age! years old.";
    std::string prefix = ""; // no prefix
    std::string suffix = ""; // no suffix
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = helper::replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is !name! and I am !age! years old.";
    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, work)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {"include", "include/**"}, {"include/*1.hpp"});
    std::set<std::string> expected = {"include/stuff.hpp", "include"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, empty_includes_with_excludes)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {}, {"src/**", "test/*", "include/stuff.hpp"});
    std::set<std::string> expected = {};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, includes_with_empty_excludes)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {"test/**", "include", "src/*"}, {});
    std::set<std::string> expected = {"test/test_path", "test/test_path/test4.cpp", "test/test1.cpp", "test/test2.cpp", "test/test3.cpp",
        "src/main.cpp", "src/temp.cpp", "include"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, includes_and_excludes)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {"test/**", "include", "src/*"}, {"test/test*", "src/temp.cpp"});
    std::set<std::string> expected = {"test/test_path/test4.cpp", "src/main.cpp", "include"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, include_all)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {"**"}, {});
    std::set<std::string> expected = {"include", "include/stuff.hpp", "include/stuff1.hpp", "src", "src/main.cpp", "src/temp.cpp",
     "test", "test/test_path", "test/test_path/test4.cpp", "test/test1.cpp", "test/test2.cpp", "test/test3.cpp", "CMakeLists.txt"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, include_all_exclude_all)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {"**"}, {"**"});
    std::set<std::string> expected = {};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, include_non_patterns)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {"src/main.cpp", "src/temp.cpp", "test"}, {});
    std::set<std::string> expected = {"src/main.cpp", "src/temp.cpp", "test"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, include_and_exclude_non_patterns)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = helper::matchPaths(helper::getPaths(template_p, template_p), {"src/main.cpp", "src/temp.cpp", "test"}, {"src/main.cpp", "test"});
    std::set<std::string> expected = {"src/temp.cpp"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(replaceVariablesInAllFilenames, working)
{
    std::string testing_path = path::joinPath(test_path, "test_suites/replace_filenames");
    std::set<std::string> paths = {"!project!", "!project!/!project!.py"};
    std::unordered_map<std::string, std::string> keyval = {{"project", "pypy"}, {"unused", "var"}};

    helper::replaceVariablesInAllFilenames(testing_path, paths, keyval, "!", "!");

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy")));

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!/!project!.py")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy/pypy.py")));

    path::rename(path::joinPath(testing_path, "pypy/pypy.py"), "!project!.py");
    path::rename(path::joinPath(testing_path, "pypy"), "!project!");
}

TEST(replaceVariablesInAllFilenames, working_includes_and_excludes)
{
    std::string testing_path = path::joinPath(test_path, "test_suites/replace_filenames");
    std::set<std::string> includes = {"!project!", "!project!/**"};
    std::set<std::string> excludes = {"!project!/temp*"};
    std::set<std::string> paths = helper::matchPaths(helper::getPaths(testing_path, testing_path), includes, excludes);

    EXPECT_EQ(paths, normalizePaths(std::set<std::string>({"!project!", "!project!/!project!.py"})));

    std::unordered_map<std::string, std::string> keyval = {{"project", "pypy"}, {"unused", "var"}};

    helper::replaceVariablesInAllFilenames(testing_path, paths, keyval, "!", "!");

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy")));

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!/!project!.py")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy/pypy.py")));

    path::rename(path::joinPath(testing_path, "pypy/pypy.py"), "!project!.py");
    path::rename(path::joinPath(testing_path, "pypy"), "!project!");
}