#include "os.hpp"
#include "helper.hpp"
#include "fmatch.hpp"
#include "format.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;
namespace path = os::path;
namespace fs = std::filesystem;

void printKeyval(const std::unordered_map<std::string, std::string>& keyval)
{
    for(const auto& i : keyval) {
        std::cout << i.first << ": " << i.second << std::endl;
    }
}

void showConfig(const json& config)
{
    std::vector<std::vector<std::string>> table = {{"Key", "Value"}};
    for(auto it = config.begin(); it != config.end(); it++) {
        std::vector<std::string> temp = {it.key(), it.value()};
        table.push_back(temp);
    }

    format::Table t(table, '-', '|', 3);
    t.print();
}

void setConfigValue(json& config, const std::vector<std::string>& config_key_values)
{
    for(int i = 0; i < config_key_values.size(); i++) {
        std::vector<std::string> keyval = split(config_key_values[i], "=");

        if(!config.contains(keyval[0])) {
            std::cout << "[WARNING] Key \"" << keyval[0] << "\" does not exist" << std::endl;
            continue;
        }

        config[keyval[0]] = keyval[1];
    }
}

std::string readTextFromFile(const std::string& file_path)
{
    std::ifstream file(file_path);
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    return ss.str();
}

json readJsonFromFile(const std::string& file)
{
    json j;
    std::ifstream i(file);
    i >> j;
    i.close();
    
    return j;
}

void writeTextToFile(const std::string& str, const std::string& file_path)
{
    std::ofstream o(file_path);
    o << str;
    o.close();
}

void writeJsonToFile(const json& j, const std::string& file, int indent)
{
    std::ofstream o(file);
    o << std::setw(indent) << j;
    o.close();
}

void writeJsonToFile(const nlohmann::ordered_json& j, const std::string& file, int indent)
{
    std::ofstream o(file);
    o << std::setw(indent) << j;
    o.close();
}

std::set<std::string> jsonListToSet(const nlohmann::json& j)
{
    std::set<std::string> s;

    if(j.is_array()) {
        for(int i = 0; i < j.size(); i++) {
            s.insert(j[i]);
        }
    } else if(j.is_object()) {
        for(auto it = j.begin(); it != j.end(); it++) {
            s.insert(it.key());
        }
    }

    return s;
}

std::unordered_set<std::string> jsonListToUnorderedSet(const nlohmann::json& j)
{
    std::unordered_set<std::string> s;

    if(j.is_array()) {
        for(int i = 0; i < j.size(); i++) {
            s.insert(j[i]);
        }
    } else if(j.is_object()) {
        for(auto it = j.begin(); it != j.end(); it++) {
            s.insert(it.key());
        }
    }

    return s;
}

std::vector<std::string> jsonObjectToList(const nlohmann::json& j)
{
    std::vector<std::string> v;

    if(j.is_array()) {
        for(int i = 0; i < j.size(); i++) {
            v.push_back(j[i]);
        }
    } else if(j.is_object()) {
        for(auto it = j.begin(); it != j.end(); it++) {
            v.push_back(it.key());
        }
    }

    return v;
}

std::string listToString(const std::vector<std::string>& v, const std::string& separator)
{
    std::string s;

    for(int i = 0; i < v.size(); i++) {
        s.append(v[i]);
        if(i < v.size()-1) {
            s.append(separator);
        }
    }

    return s;
}

std::vector<std::string> split(const std::string& str, const std::string& separators)
{
    std::vector<std::string> v;
    if(str.empty()) {
        return v;
    }

    std::unordered_set<char> reg;
    for(int i = 0; i < separators.size(); i++) {
        reg.insert(separators[i]);
    }

    std::string temp;
    int i = 0;
    while(i < str.size()) {
        while(reg.count(str[i]) > 0) {
            i++;
        }
        if(i == str.size()-1 || reg.count(str[i+1]) > 0) {
            temp += str[i];
            v.push_back(temp);
            temp.clear();
            i += 2;
        } else {
            temp += str[i];
            i++;
        }
    }
    return v;
}

std::unordered_map<std::string, std::string> mapKeyValues(const std::vector<std::string>& keyvals)
{
    std::unordered_map<std::string, std::string> m;
    for(const auto& i : keyvals) {
        std::vector<std::string> splitted = split(i, "=");

        if(splitted.size() < 2) {
            continue;
        }

        m.insert({splitted[0], splitted[1]});
    }

    return m;
}

bool equalVariables(const nlohmann::json& j, const std::unordered_map<std::string, std::string>& keyvals, bool error_message)
{
    std::unordered_set<std::string> vars = jsonListToUnorderedSet(j);
    std::set<std::string> unknown_vars;

    for(const auto& i : keyvals) {
        if(vars.count(i.first) < 1) {
            unknown_vars.insert(i.first);
        }
    }

    if(unknown_vars.empty()) {
        return true;
    }
 
    if(error_message) {
        std::string message = "[ERROR] Unknown variable(s): ";
        for(const auto& i : unknown_vars) {
            message += "\"" + i + "\", ";
        }
        message.pop_back();
        message.pop_back();
        std::cout << message << std::endl;
    }

    return false;
}

std::string replaceVariables(const std::string& str, 
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix)
{
    if(prefix.empty() || suffix.empty()) {
        return str;
    }

    int max_var_size = 0;
    for(const auto& i : keyval) {
        if(i.first.size() > max_var_size) {
            max_var_size = i.first.size();
        }
    }

    std::string new_str;

    // iterate through "str" until prefix is found
    for(int i = 0; i < str.size(); i++) {
        // check if current character is equivalent to the first character of prefix then check if it is the prefix itself
        if(str[i] == prefix[0] && str.substr(i, prefix.size()) == prefix) {
            std::string var;
            int j = i + prefix.size(); // skip the prefix
            int counter = 0;
            bool has_suffix = false;

            while(j < str.size() && counter <= max_var_size) {
                // check if the current character is equivalent to the first character of suffix 
                // then check if it is the suffix itself
                if(str[j] == suffix[0] && str.substr(j, suffix.size()) == suffix) {
                    has_suffix = true;
                    break;
                }
                var.push_back(str[j]);
                j++;
                counter++;
            }

            // if the variable is in keyval then append it to new_str
            if(has_suffix && keyval.count(var) > 0) {
                new_str.append(keyval.at(var));
            }

            // move to the last character of the suffix (-1 to not skip a character in the next iteration)
            if(has_suffix) {
                i = j + suffix.size() - 1;
            } else {
                new_str.append(prefix);
                i += prefix.size() - 1;
            }
            continue;
        }

        new_str.push_back(str[i]);
    }

    return new_str;
}

void replaceVariablesInFile(const std::string& file_path, 
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix)
{
    std::string str = replaceVariables(readTextFromFile(file_path), keyval, prefix, suffix);
    writeTextToFile(str, file_path);
}

void replaceVariablesInAllFiles(const std::string& root_path, const std::set<std::string>& paths,
                            const std::unordered_map<std::string, std::string>& keyval, 
                            const std::string& prefix, const std::string& suffix)
{
    for(const auto& i : paths) {
        std::string path = path::joinPath(root_path, i);
        
        if(path::isDirectory(path)) {
            continue;
        }

        replaceVariablesInFile(path, keyval, prefix, suffix);
    }
}

void replaceVariablesInAllFilenames(const std::string& root_path, const std::set<std::string>& paths,
                            const std::unordered_map<std::string, std::string>& keyval,
                            const std::string& prefix, const std::string& suffix)
{
    // Needs to change names from bottom to top of file tree to avoid error
    // Ordered set is used to fix this
    for(auto i = paths.rbegin(); i != paths.rend(); i++) {
        std::string filename = path::filename(*i);
        std::string path = path::joinPath(root_path, *i);

        if(!path::exists(path)) {
            continue;
        }

        std::string new_filename = replaceVariables(filename, keyval, prefix, suffix);

        if(filename == new_filename) {
            continue;
        }

        path::rename(path, new_filename);
    }
}

std::set<std::string> getPaths(const std::string& path, const std::string& relative_to)
{
    std::set<std::string> paths;
    for(const auto& i : fs::recursive_directory_iterator(path)) {
        std::string p = relative_to.empty() ? i.path().string() : path::relativePath(i, path);
        paths.insert(p);
    }

    return paths;
}

std::set<std::string> matchPaths(const std::set<std::string>& included_paths, const std::set<std::string>& include, const std::set<std::string>& exclude)
{
    std::set<std::string> matched;
    for(const auto& str : included_paths) {
        bool included = false;
        for(const auto& pattern : include) {
            if(fmatch::match(str, pattern)) {
                included = true;
                break;
            }
        }

        if(included) {
            bool excluded = false;
            for(const auto& pattern : exclude) {
                if(fmatch::match(str, pattern)) {
                    excluded = true;
                    break;
                }
            }

            if(!excluded) {
                matched.insert(str);
            }
        }
    }

    return matched;
}