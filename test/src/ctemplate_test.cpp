#include "gtest/gtest.h"
#include "os.hpp"
#include "helper.hpp"



TEST(split, splitting)
{
    std::vector<std::string> s = split("key=value", "=");
    EXPECT_EQ(s, std::vector<std::string>({"key", "value"}));
}