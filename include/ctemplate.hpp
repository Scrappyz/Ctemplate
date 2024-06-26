#pragma once

#include <string>
#include <vector>

void initTemplate(const std::string& template_dir, const std::string& template_name, const std::string& init_to);
void addTemplate(const std::string& template_dir, const std::string& path_to_add, const std::string& name, const std::string& desc, const std::string& container_name);
void removeTemplates(const std::string& template_dir, const std::vector<std::string>& templates);
void listTemplates(const std::string& template_dir, const std::string& container_name);