#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

class Config {
    private:
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config;

    public:
        // Constructors
        Config() : config() {}

        Config(const std::string& config_path) : config()
        {
            setConfig(config_path);
        }

        // Getters
        const std::string& getValueOf(const std::string& key) const
        {
            return config.at("").at(key);
        }

        const std::string& getValueOf(const std::string& section, const std::string& key) const
        {
            return config.at(section).at(key);
        }

        // Setters
        void setConfig(const std::string& config_path)
        {
            std::ifstream file(config_path);
            std::string temp;

            if(file.is_open()) {
                std::string section;
                while(getline(file, temp)) {
                    std::string key, value;

                    if(temp.empty()) {
                        continue;
                    }

                    int i = 0;
                    while(i < temp.size() && temp[i] == ' ') { // skip whitespaces
                        i++;
                    }

                    if(temp[i] == '#') { // is a comment
                        continue;
                    }

                    if(temp[i] == '[') {
                        section.clear();
                        i++;
                        while(i < temp.size() && temp[i] != ']') {
                            section.push_back(temp[i]);
                            i++;
                        }

                        if(config.count(section) < 1) {
                            config.insert({section, std::unordered_map<std::string, std::string>()});
                        }
                    } else {
                        bool is_string = false;
                        if(temp[i] == '"' || temp[i] == '\'') {
                            is_string = true;
                            i++;
                        } 

                        while(i < temp.size()) {
                            if(!is_string && temp[i] == ' ' || (temp[i] == '\"' || temp[i] == '\'' || temp[i] == '=')) {
                                break;
                            }
                            key.push_back(temp[i]);
                            i++;
                        }

                        while(i < temp.size() && temp[i] != '=') { // move to '='
                            i++;
                        }

                        if(i >= temp.size()) { // is not a key value pair
                            continue;
                        }
                        i++;

                        while(i < temp.size() && temp[i] == ' ') { // skip whitespaces
                            i++;
                        }

                        is_string = false;
                        if(temp[i] == '"' || temp[i] == '\'') {
                            is_string = true;
                            i++;
                        }

                        while(i < temp.size()) {
                            if(!is_string && temp[i] == ' ' || (temp[i] == '\"' || temp[i] == '\'')) {
                                break;
                            }
                            value.push_back(temp[i]);
                            i++;
                        }

                        if(config.count(section) < 1) {
                            config.insert({section, std::unordered_map<std::string, std::string>()});
                        }
                        config.at(section).insert({key, value});
                    }
                }
            } else {
                throw std::runtime_error("[Error] Could not open configuration file at \"" + config_path + "\"");
            }
        }
};