#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "os.hpp"
#include "helper.hpp"

using json = nlohmann::json;
namespace path = os::path;
namespace fs = std::filesystem;

void showConfig(const json& config, int space_before)
{
    for(const auto& i : config.items()) {
        std::cout << std::string(space_before, ' ') << i.key() << " = " << i.value() << std::endl;
    }
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

json readJsonFromFile(const std::string& file)
{
    json j;
    std::ifstream i(file);
    i >> j;
    i.close();
    
    return j;
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

std::unordered_set<std::string> jsonArrayToSet(const json& j)
{
    std::unordered_set<std::string> s;

    if(!j.is_array()) {
        return s;
    }

    for(int i = 0; i < j.size(); i++) {
        s.insert(j[i]);
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

std::vector<std::string> getAlignedOutput(const std::vector<std::vector<std::string>>& v, int space)
{
    std::stringstream s;
    std::vector<std::string> table;
    
    for(int i = 0; i < v.size(); i++) {
        std::string temp;
        s << std::left << std::setfill(' ') << std::setw(space);
        for(int j = 0; j < v[i].size(); j++) {
            s << v[i][j];
            temp.append(s.str());
            s.str(std::string());
        }
        table.push_back(temp);
    }

    return table;
}

std::vector<std::vector<std::string>> makeTable(const std::vector<std::vector<std::string>>& v)
{
    // Unfinished
    std::vector<std::vector<std::string>> cleaned;
    std::vector<std::string> overflow;
    for(int i = 0; i < v.size(); i++) {
        for(int j = 0; j < v[i].size(); j++) {
            std::string str = v[i][j];
            int endlines = 0;
            std::string temp;
            for(int k = 0; k < str.size(); k++) {
                if(str[k] == '\n') {
                    endlines++;
                    if(!temp.empty()) {
                        overflow.push_back(temp);
                    }
                    temp.clear();
                    continue;
                }

                if(endlines > 0) {
                    temp.push_back(str[k]);
                }
            }
            if(!temp.empty() && endlines > 0) {
                overflow.push_back(temp);
            }
        }
    }

    return cleaned;
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
    std::ifstream file(file_path);
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    std::string str = replaceVariables(ss.str(), keyval, prefix, suffix);

    std::ofstream o(file_path);
    o << str;
    o.close();
}

void replaceVariablesInAllFiles(const std::string& root_path, const std::unordered_set<std::string>& paths,
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

// Compiles included from excluded paths
std::unordered_set<std::string> compileIncludedPaths(const std::string& root_path, const std::unordered_set<std::string>& includes, const std::unordered_set<std::string>& excludes)
{
    std::unordered_set<std::string> clean_includes;

    if(includes.empty()) {
        for(const auto& i : fs::recursive_directory_iterator(root_path)) {
            std::string path = i.path().string();
            clean_includes.insert(path::relativePath(path, root_path));
        }
    } else {
        clean_includes = getPathsForCompile(root_path, includes);
    }

    std::unordered_set<std::string> clean_excludes = getPathsForCompile(root_path, excludes);

    if(clean_excludes.empty()) {
        return clean_includes;
    }

    std::unordered_set<std::string> compiled;
    for(const auto& i : clean_includes) {
        if(clean_excludes.count(i) > 0) {
            continue;
        }

        compiled.insert(i);
    }

    return compiled;
}

// Helper for compileIncludedPaths
// Gets all paths relative to "root_path" from a given set of directories
// {"src"} to {"src", "src/main.cpp", "src/temp.cpp"}
std::unordered_set<std::string> getPathsForCompile(const std::string& root_path, const std::unordered_set<std::string>& s)
{
    std::unordered_set<std::string> paths;
    
    for(const auto& i : s) {
        std::string path = path::joinPath(root_path, i);
        std::string relative_path = path::relativePath(path::joinPath(root_path, i), root_path);

        if(!path::exists(path)) {
            continue;
        }

        if(paths.count(relative_path) > 0) {
            continue;
        }

        paths.insert(relative_path);

        if(!path::isDirectory(path)) {
            continue;
        }

        for(auto j = fs::recursive_directory_iterator(path); j != fs::recursive_directory_iterator(); j++) {
            std::string path1 = j->path().string();
            std::string relative_path1 = path::relativePath(path1, root_path);

            paths.insert(relative_path1);
        }
    }

    return paths;
}