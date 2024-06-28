#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "json.hpp"

void showConfig(const nlohmann::json& config, int space_before = 0);
void setConfigValue(nlohmann::json& config, const std::vector<std::string>& config_key_values);
nlohmann::json readJsonFromFile(const std::string& file);
void writeJsonToFile(const nlohmann::json& j, const std::string& file, int indent = 0);
void writeJsonToFile(const nlohmann::ordered_json& j, const std::string& file, int indent = 0);
std::unordered_set<std::string> jsonArrayToSet(const nlohmann::json& j);
std::set<std::string> jsonArrayToOrderedSet(const nlohmann::json& j);

template<typename T>
std::set<T> convertUnorderedSetToSet(const std::unordered_set<T>& s)
{
    std::set<T> ss;
    for(const auto& i : s) {
        ss.insert(i);
    }
    return ss;
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter);
std::unordered_map<std::string, std::string> mapKeyValues(const std::vector<std::string>& keyvals);
std::vector<std::string> getAlignedOutput(const std::vector<std::vector<std::string>>& v, int space);
std::vector<std::vector<std::string>> makeTable(const std::vector<std::vector<std::string>>& v);

std::string replaceVariables(const std::string& str, 
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix);

void replaceVariablesInFile(const std::string& file_path, 
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix);

void replaceVariablesInAllFiles(const std::string& root_path, const std::unordered_set<std::string>& paths,
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix);

void replaceVariablesInAllFilenames(const std::string& root_path, const std::set<std::string>& paths,
                            const std::unordered_map<std::string, std::string>& keyval,
                            const std::string& prefix, const std::string& suffix);

std::unordered_set<std::string> getIncludedPaths(const std::string& root_path, 
                            const std::unordered_set<std::string>& includes, 
                            const std::unordered_set<std::string>& excludes);
                            
std::unordered_set<std::string> getAllPaths(const std::string& root_path, const std::unordered_set<std::string>& s);