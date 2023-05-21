#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "path.hpp"

using namespace std;

template<typename K, typename V>
void print(const unordered_map<K, V>& m)
{
    for(const auto& i : m) {
        cout << i.first << " | " << i.second << endl;
    }
}

string getConfigPath()
{
    #ifdef NDEBUG
        return joinPath(getSourcePath(), "config.txt");
    #else
        return joinPath(getSourcePath(), "../../config.txt");
    #endif
}

unordered_map<string, string> parseConfigFile(const string& config_file)
{
    unordered_map<string, string> m;
    ifstream config(config_file);
    if(config.is_open()) {
        string temp;
        while(getline(config, temp)) {
            if(temp[0] == '#' || temp[0] == ' ') {
                continue;
            }
            string key, value;
            int i = 0;
            while(i < temp.size() && (temp[i] != '=' && temp[i] != ':')) { // move forward until we reach a separator
                key.push_back(temp[i]);
                i++;
            }
            while(key.back() == ' ') { // remove trailing whitespace
                key.pop_back();
            }
            i++; // move forward
            while(i < temp.size() && temp[i] == ' ') { // ignore whitespaces
                i++;
            }
            if(temp[i] == '"') {
                i++; // move forward
                while(i < temp.size() && temp[i] != '"') {
                    value.push_back(temp[i]);
                    i++;
                }
            } else {
                while(i < temp.size() && temp[i] != ' ') {
                    value.push_back(temp[i]);
                    i++;
                }
            }
            while(value.back() == ' ') {
                value.pop_back();
            }
            m.insert({key, value});
        }
    } else {
        cerr << "[Error] Could not open configuration file" << endl;
    }
    return m;
}

void copyDirectory(const string& source, const string& destination, bool recurse = true)
{
    if(filesystem::exists(source) && filesystem::is_directory(source)) {
        if(recurse) {
            for(const auto& entry : filesystem::recursive_directory_iterator(source)) {
                const string copy_to = joinPath(destination, relativePath(entry.path(), source));
                if(filesystem::is_directory(entry.path())) {
                    filesystem::create_directories(copy_to);
                } else {
                    std::ifstream src(entry.path(), std::ios::binary);
                    std::ofstream dst(copy_to, std::ios::binary);
                    dst << src.rdbuf();
                }
            }
        } else {
            for(const auto& entry : filesystem::directory_iterator(source)) {
                const string copy_to = joinPath(destination, relativePath(entry.path(), source));
                if(filesystem::is_directory(entry.path())) {
                    filesystem::create_directories(copy_to);
                } else {
                    std::ifstream src(entry.path(), std::ios::binary);
                    std::ofstream dst(copy_to, std::ios::binary);
                    dst << src.rdbuf();
                }
            }
        }
    } else {
        cerr << "[Error] Could not find template \"" << getFilename(source) << "\"" << endl;
    }
}

int main(int argc, char** argv)
{
    vector<string> args;
    args.assign(argv+1, argv+argc);
    unordered_map<string, string> config = parseConfigFile(getConfigPath());
    string program_name = argv[0];

    if(args.empty() || config.empty()) {
        return 0;
    }
    
    string template_dir = config.empty() ? joinPath("Templates", args[0]) : joinPath(config.at("TemplateDirectory"), args[0]);
    copyDirectory(joinPath(getSourcePath(), template_dir), getCurrentPath());
    return 0;
}