#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

class Config {
    protected:
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config;

    public:
        // Constructors
        Config() : config() 
        {
            config.insert({"", std::unordered_map<std::string, std::string>()});
        }

        Config(const std::string& config_path) : config()
        {
            setConfigFromFile(config_path);
        }

        // Getters
        const std::string& getValue(const std::string& key) const
        {
            return config.at("").at(key);
        }

        const std::string& getValue(const std::string& section, const std::string& key) const
        {
            return config.at(section).at(key);
        }

        // Setters
        void setConfigFromFile(const std::string& config_path)
        {
            config.clear();
            config.insert({"", std::unordered_map<std::string, std::string>()});
            std::ifstream file(config_path);
            std::string temp;

            if(!file.is_open()) {
                throw std::runtime_error("[Error] Could not open configuration file at \"" + config_path + "\"");
            }

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

                    continue;
                }

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

        // Modifiers
        void addSection(const std::string& new_section)
        {
            config.insert({new_section, std::unordered_map<std::string, std::string>()});
        }

        void addKey(const std::string& new_key)
        {
            addKey("", new_key);
        }

        void addKey(const std::string& section, const std::string& new_key)
        {
            config.at(section).insert({new_key, ""});
        }

        void addKeyValue(const std::string& new_key, const std::string& new_val)
        {
            addKeyValue("", new_key, new_val);
        }

        void addKeyValue(const std::string& section, const std::string& new_key, const std::string& new_val)
        {
            config.at(section).insert({new_key, new_val});
        }

        void removeSection(const std::string& section)
        {
            config.erase(section);
        }

        void removeKey(const std::string& key)
        {
            removeKey("", key);
        }

        void removeKey(const std::string& section, const std::string& key)
        {
            config.at(section).erase(key);
        }

        void modifySectionName(const std::string& section, const std::string& new_section)
        {
            std::unordered_map<std::string, std::string> keys = config.at(section);
            config.erase(section);
            config.insert({new_section, keys});
        }

        void modifyKeyName(const std::string& key, const std::string& new_key)
        {
            modifyKeyName("", key, new_key);
        }

        void modifyKeyName(const std::string& section, const std::string& key, const std::string& new_key)
        {
            std::string value = config.at(section).at(key);
            config.at(section).erase(key);
            config.at(section).insert({new_key, value});
        }

        void modifyKeyValue(const std::string& key, const std::string& new_val)
        {
            modifyKeyValue("", key, new_val);
        }

        void modifyKeyValue(const std::string& section, const std::string& key, const std::string& new_val)
        {
            config.at(section)[key] = new_val;
        }

        // Checkers
        bool doesSectionExist(const std::string& section) const
        {
            return config.count(section) > 0;
        }

        bool doesKeyExist(const std::string& key) const
        {
            return config.at("").count(key) > 0;
        }

        bool doesKeyExist(const std::string& section, const std::string& key) const
        {
            return config.at(section).count(key) > 0;
        }

        bool doesKeyHaveValue(const std::string& key) const 
        {
            return !config.at("").at(key).empty();
        }

        bool doesKeyHaveValue(const std::string& section, const std::string& key) const
        {
            return !config.at(section).at(key).empty();
        }

        // File Handling
        void saveConfigToFile(const std::string& config_path) const
        {
            std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> temp;

            for(const auto& i : config) {
                std::string section = i.first;

                if(config.at(section).empty()) {
                    continue;
                }

                std::vector<std::pair<std::string, std::string>> keyval;
                for(const auto& j : i.second) {
                     keyval.push_back(std::make_pair(j.first, j.second));
                }
                
                temp.push_back(std::make_pair(section, keyval));
            }

            std::ofstream output(config_path);

            for(int i = temp.size()-1; i >= 0; i--) {
                if(!temp[i].first.empty()) {
                    output << "[" << temp[i].first << "]" << std::endl;
                }

                for(int j = temp[i].second.size()-1; j >= 0; j--) {
                    size_t key_space = temp[i].second[j].first.find(' ');
                    size_t value_space = temp[i].second[j].second.find(' ');
                    std::string key = (key_space == std::string::npos ? temp[i].second[j].first : "\"" + temp[i].second[j].first + "\"");
                    std::string value = (value_space == std::string::npos ? temp[i].second[j].second : "\"" + temp[i].second[j].second + "\"");
                    output << key << " = " << value << std::endl;
                }

                output << std::endl;
            }

            output.close();
        }

        // Operators
        bool operator==(const Config& config) const
        {
            return this->config == config.config;
        }
};