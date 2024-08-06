#pragma once

#include "json.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

void printKeyval(const std::unordered_map<std::string, std::string>& keyval);
void showConfig(const nlohmann::json& config, int space_before = 0);
void setConfigValue(nlohmann::json& config, const std::vector<std::string>& config_key_values);
std::string readTextFromFile(const std::string& file_path);
nlohmann::json readJsonFromFile(const std::string& file);
void writeTextToFile(const std::string& str, const std::string& file_path);
void writeJsonToFile(const nlohmann::json& j, const std::string& file, int indent = 0);
void writeJsonToFile(const nlohmann::ordered_json& j, const std::string& file, int indent = 0);

// Works on arrays and objects
std::set<std::string> jsonListToSet(const nlohmann::json& j);
std::unordered_set<std::string> jsonListToUnorderedSet(const nlohmann::json& j);
std::vector<std::string> jsonObjectToList(const nlohmann::json& j);
std::string listToString(const std::vector<std::string>& v, const std::string& separator = " ");

template<typename T>
std::set<T> arrayToSet(const std::vector<T>& v)
{
    std::set<T> s;
    for(const auto& i : v) {
        s.insert(i);
    }

    return s;
}

template<typename T>
std::set<T> unorderedSetToSet(const std::unordered_set<T>& s)
{
    std::set<T> ss;
    for(const auto& i : s) {
        ss.insert(i);
    }
    return ss;
}

template<typename T>
std::unordered_set<T> setToUnorderedSet(const std::set<T>& s)
{
    std::unordered_set<T> ss;
    for(const auto& i : s) {
        ss.insert(i);
    }
    return ss;
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter);
std::unordered_map<std::string, std::string> mapKeyValues(const std::vector<std::string>& keyvals);
bool equalVariables(const nlohmann::json& j, const std::unordered_map<std::string, std::string>& keyvals, bool error_message = false);

std::string replaceVariables(const std::string& str, 
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix);

void replaceVariablesInFile(const std::string& file_path, 
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix);

void replaceVariablesInAllFiles(const std::string& root_path, const std::set<std::string>& paths,
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix);

void replaceVariablesInAllFilenames(const std::string& root_path, const std::set<std::string>& paths,
                            const std::unordered_map<std::string, std::string>& keyval,
                            const std::string& prefix, const std::string& suffix);

std::set<std::string> getPaths(const std::string& path, const std::string& relative_to = "");
std::set<std::string> matchPaths(const std::set<std::string>& paths, const std::set<std::string>& include, const std::set<std::string>& exclude);