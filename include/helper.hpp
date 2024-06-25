#pragma once

#include <vector>
#include <unordered_map>
#include "json.hpp"

nlohmann::json readJsonFromFile(const std::string& file);
void writeJsonToFile(const nlohmann::json& j, const std::string& file, int indent = 0);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);
std::unordered_map<std::string, std::string> mapKeyValues(const std::vector<std::string>& keyvals);
std::vector<std::string> getAlignedOutput(const std::vector<std::vector<std::string>>& v, int space);
std::vector<std::vector<std::string>> makeTable(const std::vector<std::vector<std::string>>& v);
std::string replaceVariables(const std::string& str, const std::unordered_map<std::string, std::string>& keyval, const std::string& prefix, const std::string& suffix);
void replaceVariablesInFile(const std::string& file_path, const std::unordered_map<std::string, std::string>& keyval, const std::string& prefix, const std::string& suffix);