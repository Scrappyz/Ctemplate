#include "gtest/gtest.h"
#include "os.hpp"
#include "helper.hpp"

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