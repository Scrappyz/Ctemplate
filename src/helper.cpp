#include <fstream>
#include <unordered_set>
#include <iomanip>
#include <sstream>
#include "helper.hpp"

using json = nlohmann::json;

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

std::string replaceVariables(const std::string& str, const std::vector<std::string>& keyval, const std::string& prefix, const std::string& suffix)
{
    if(prefix.empty() || suffix.empty()) {
        return str;
    }

    std::string new_str;

    // iterate through "str" until prefix is found
    std::string var;
    int start_var_pos = -1;
    for(int i = 0; i < str.size(); i++) {
        if(str[i] == prefix[0] && str.substr(i, prefix.size()) == prefix) {
            start_var_pos = i;
            std::string temp;
            int j = start_var_pos + prefix.size();
            while(j < str.size()) {
                if(str[j] == suffix[0] && str.substr(j, suffix.size()) == suffix) {
                    break;
                }
                temp.push_back(str[j]);
                j++;
            }

        }
    }

    return new_str;
}