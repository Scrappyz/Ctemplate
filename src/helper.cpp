#include "helper.hpp"
#include <fstream>
#include <unordered_set>

using json = nlohmann::json;

json readJsonFromFile(const std::string& file)
{
    json j;
    std::ifstream i(file);
    i >> j;
    i.close();
    
    return j;
}

void writeJsonToFile(const nlohmann::json& j, const std::string& file, int indent)
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