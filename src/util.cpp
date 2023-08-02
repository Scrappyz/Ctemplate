#include "util.hpp"
#include "path.hpp"

std::string getSpaces(int spaces, const std::string& str)
{
    int actual_spaces = spaces - str.size();
    if(actual_spaces < 5) {
        actual_spaces = 5;
    }
    return std::string(actual_spaces, ' ');
}

std::string trim(const std::string& str)
{
    std::string trimmed;
    trimmed.reserve(str.size());
    int i = 0;
    while(i < str.size() && str[i] == ' ') {
        i++;
    }
    while(i < str.size()) {
        if(str[i] == ' ') {
            while(i < str.size() && str[i] == ' ') {
                i++;
            }
            if(i >= str.size()) {
                break;
            } else {
                trimmed.push_back(' ');
            }
        }
        trimmed.push_back(str[i]);
        i++;
    }
    return trimmed;
}

std::string getTemplateDirectory(const Config& config)
{
    std::string config_template_dir = config.getValue("template_directory");
    return path::isAbsolutePath(config_template_dir) ? config_template_dir : path::joinPath(path::sourcePath(), config_template_dir);
}

std::string getConfigValue(const Config& config, const std::string& section, const std::string& key)
{
    if(config.doesSectionExist(section) && config.doesKeyExist(key)) {
        return config.getValue(section, key);
    }
    return std::string();
}

std::string getConfigValue(const Config& config, const std::string& key)
{
    return getConfigValue(config, "", key);
}