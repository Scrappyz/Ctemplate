#pragma once

#include <vector>
#include "json.hpp"

nlohmann::json readJsonFromFile(const std::string& file);
void writeJsonToFile(const nlohmann::json& j, const std::string& file, int indent = 0);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);
std::string alignedSpace(int total_space, int occupied_space = 0);
std::vector<std::string> getAlignedOutput(const std::vector<std::vector<std::string>>& v, int space);