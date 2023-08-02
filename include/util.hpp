#pragma once

#include "config.hpp"

std::string getSpaces(int spaces, const std::string& str);
std::string trim(const std::string& str);
std::string getTemplateDirectory(const Config& config);
std::string getConfigValue(const Config& config, const std::string& section, const std::string& key);
std::string getConfigValue(const Config& config, const std::string& key);