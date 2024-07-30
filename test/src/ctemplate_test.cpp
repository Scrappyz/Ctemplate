#include "gtest/gtest.h"
#include "ctemplate.hpp"
#include "helper.hpp"
#include "os.hpp"

namespace path = os::path;

std::string test_path = path::joinPath(path::sourcePath(), "../test_path");
std::string template_path = path::joinPath(test_path, "templates");
std::string temp_path = path::joinPath(test_path, "temp");

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

// TEST(initTemplate, working)
// {
//     std::string tp = path::joinPath(template_path, "py");
//     std::string t_path = path::joinPath(test_path, "testing/init_template");
//     std::unordered_map<std::string, std::string> keyval = {{"project", "hello_world"}, {"name", "User"}};
     
//     initTemplate(tp, ".ctemplate", t_path, keyval);

//     ASSERT_TRUE(path::exists(path::joinPath(t_path, "hello_world")));
//     ASSERT_TRUE(path::exists(path::joinPath(t_path, "hello_world/hello_world.py")));


// }

TEST(addTemplate, adding)
{
    std::string add_path = path::joinPath(template_path, "t1");
    std::string name = "testing";
    std::string desc = "wassup boi";
    std::string container_name = ".ctemplate";
    addTemplate(template_path, add_path, name, desc, container_name);

    std::string new_template = path::joinPath(template_path, "testing");
    ASSERT_TRUE(path::exists(new_template));
    ASSERT_TRUE(path::exists(path::joinPath(new_template, container_name)));
    
    path::remove(new_template);
}

TEST(addTemplate, empty_name)
{
    std::string add_path = path::joinPath(template_path, "t1");
    std::string name = "testing";
    std::string desc = "wassup boi";
    std::string container_name = ".ctemplate";
    addTemplate(template_path, add_path, "", desc, container_name);

    ASSERT_TRUE(!path::exists(name));
}

TEST(addTemplate, existing_container)
{
    std::string add_path = path::joinPath(template_path, "t1");
    std::string name = "testing";
    std::string desc = "wassup boi";
    std::string container_name = ".ctemplate";
    addTemplate(template_path, add_path, name, desc, container_name);

    std::string new_template = path::joinPath(template_path, "testing");
    std::string new_container = path::joinPath(new_template, ".ctemplate");
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
    std::vector<std::string> s = split("key=value", "=");
    EXPECT_EQ(s, std::vector<std::string>({"key", "value"}));
}

TEST(mapKeyValues, mapping)
{
    std::vector<std::string> s = {"project=hello", "name=johnny sins", "order=asc", "something"};
    std::unordered_map<std::string, std::string> m = mapKeyValues(s);
    std::unordered_map<std::string, std::string> expected_m = {{"project", "hello"}, {"name", "johnny sins"}, {"order", "asc"}};
    EXPECT_EQ(m, expected_m);
}

TEST(replaceVariables, replacing)
{
    std::string str = "My name is !name! and I am !age! years old.";
    std::string prefix = "!";
    std::string suffix = "!";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and I am 12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, longer_prefix_and_suffix)
{
    std::string str = "My name is {!name!} and I am {!age!} years old.";
    std::string prefix = "{!";
    std::string suffix = "!}";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and I am 12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, uneven_prefix_and_suffix)
{
    std::string str = "My name is {!name!}] and I am {!age!}] years old.";
    std::string prefix = "{!";
    std::string suffix = "!}]";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and I am 12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, missing_prefix)
{
    std::string str = "My name is name!}] and I am age!}] years old.";
    std::string prefix = "{!";
    std::string suffix = "!}]";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is name!}] and I am age!}] years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, missing_suffix)
{
    std::string str = "My name is {!name and I am {!age years old.";
    std::string prefix = "{!";
    std::string suffix = "!}]";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is {!name and I am {!age years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, missing_variables)
{
    std::string str = "My name is !name! and";
    std::string prefix = "!";
    std::string suffix = "!";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John and";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, endlines_in_str)
{
    std::string str = "My name is {!name!}\nand I am\n{!age!} years old.";
    std::string prefix = "{!";
    std::string suffix = "!}";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John\nand I am\n12 years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, endlines_in_var)
{
    std::string str = "My name is {!name!} and I am {!age!} years old.";
    std::string prefix = "{!";
    std::string suffix = "!}";
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John\nWick"}, {"age", "12\nteen"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is John\nWick and I am 12\nteen years old.";
    EXPECT_EQ(actual, expected);
}

TEST(replaceVariables, edge_cases)
{
    std::string str = "My name is !name! and I am !age! years old.";
    std::string prefix = ""; // no prefix
    std::string suffix = ""; // no suffix
    std::unordered_map<std::string, std::string> keyvals = {{"name", "John"}, {"age", "12"}};

    std::string actual = replaceVariables(str, keyvals, prefix, suffix);
    std::string expected = "My name is !name! and I am !age! years old.";
    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, work)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = matchPaths(getPaths(template_p, template_p), {"include", "include/**"}, {"include/*1.hpp"});
    std::set<std::string> expected = {"include/stuff.hpp", "include"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, empty_includes_with_excludes)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = matchPaths(getPaths(template_p, template_p), {}, {"src/**", "test/*", "include/stuff.hpp"});
    std::set<std::string> expected = {"CMakeLists.txt", "src", "test", "test/test_path/test4.cpp", "include", "include/stuff1.hpp"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, includes_with_empty_excludes)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = matchPaths(getPaths(template_p, template_p), {"test/**", "include", "src/*"}, {});
    std::set<std::string> expected = {"test/test_path", "test/test_path/test4.cpp", "test/test1.cpp", "test/test2.cpp", "test/test3.cpp",
        "src/main.cpp", "src/temp.cpp", "include"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(matchPaths, includes_and_excludes)
{
    std::string template_p = path::joinPath(template_path, "cpp-test");
    std::set<std::string> actual = matchPaths(getPaths(template_p, template_p), {"test/**", "include", "src/*"}, {"test/test*", "src/temp.cpp"});
    std::set<std::string> expected = {"test/test_path/test4.cpp", "src/main.cpp", "include"};
    expected = normalizePaths(expected, template_p);

    EXPECT_EQ(actual, expected);
}

TEST(replaceVariablesInAllFilenames, working)
{
    std::string testing_path = path::joinPath(test_path, "testing/replace_filenames");
    std::set<std::string> paths = {"!project!", "!project!/!project!.py"};
    std::unordered_map<std::string, std::string> keyval = {{"project", "pypy"}, {"unused", "var"}};

    replaceVariablesInAllFilenames(testing_path, paths, keyval, "!", "!");

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy")));

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!/!project!.py")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy/pypy.py")));

    path::rename(path::joinPath(testing_path, "pypy/pypy.py"), "!project!.py");
    path::rename(path::joinPath(testing_path, "pypy"), "!project!");
}

TEST(replaceVariablesInAllFilenames, working_includes_and_excludes)
{
    std::string testing_path = path::joinPath(test_path, "testing/replace_filenames");
    std::set<std::string> includes = {"!project!", "!project!/**"};
    std::set<std::string> excludes = {"!project!/temp*"};
    std::set<std::string> paths = matchPaths(getPaths(testing_path, testing_path), includes, excludes);

    EXPECT_EQ(paths, normalizePaths(std::set<std::string>({"!project!", "!project!/!project!.py"})));

    std::unordered_map<std::string, std::string> keyval = {{"project", "pypy"}, {"unused", "var"}};

    replaceVariablesInAllFilenames(testing_path, paths, keyval, "!", "!");

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy")));

    ASSERT_TRUE(!path::exists(path::joinPath(testing_path, "!project!/!project!.py")));
    ASSERT_TRUE(path::exists(path::joinPath(testing_path, "pypy/pypy.py")));

    path::rename(path::joinPath(testing_path, "pypy/pypy.py"), "!project!.py");
    path::rename(path::joinPath(testing_path, "pypy"), "!project!");
}