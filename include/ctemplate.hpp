#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

void initTemplate(const std::string& template_to_init, const std::set<std::string>& paths, const std::string& template_files_container_name, 
                  const std::string& path_to_init_template_to, const std::unordered_map<std::string, std::string>& keyval,
                  bool force_overwrite = false);
void initTemplate(const std::string& template_dir, const std::string& template_name, const std::set<std::string>& paths, const std::string& template_files_container_name, 
                  const std::string& path_to_init_template_to, const std::unordered_map<std::string, std::string>& keyval,
                  bool force_overwrite = false);
void initTemplate(const std::string& template_to_init, const std::string& template_files_container_name, 
                  const std::string& path_to_init_template_to, const std::unordered_map<std::string, std::string>& keyval,
                  bool force_overwrite = false);
void initTemplate(const std::string& template_dir, const std::string& template_name,
                  const std::string& template_files_container_name, const std::string& path_to_init_template_to, 
                  const std::unordered_map<std::string, std::string>& keyval, bool force_overwrite = false);
void addTemplate(const std::string& template_dir, const std::string& path_to_add, const std::string& name,
                 const std::string& author, const std::string& desc, const std::string& container_name);
void removeTemplates(const std::string& template_dir, const std::vector<std::string>& templates);
void listTemplates(const std::string& template_dir, const std::string& container_name);
void printTemplateInfo(const std::string& template_dir, const std::string& template_name, const std::string& container_name);