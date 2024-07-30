#include "ctemplate.hpp"
#include "os.hpp"
#include "json.hpp"
#include "helper.hpp"
#include <iostream>
#include <fstream>
#include <unordered_set>

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
namespace path = os::path;

void initTemplate(const std::string& template_to_init, const std::string& template_files_container_name, 
                const std::string& path_to_init_template_to, const std::unordered_map<std::string, std::string>& keyval)
{
    if(!path::exists(template_to_init)) {
        std::cout << "[ERROR] Template \"" << path::filename(template_to_init) << "\" does not exist" << std::endl;
        return;
    }

    json vars = readJsonFromFile(path::joinPath({template_to_init, template_files_container_name, "variables.json"}));
    json var_list = vars.at("variables");

    if(!equalVariables(var_list, keyval, true)) {
        return;
    }

    // Directory separator to only copy the content
    path::copy(template_to_init + path::directorySeparator(), path_to_init_template_to);

    // Remove ctemplate container from the initialized template
    path::remove(path::joinPath(path_to_init_template_to, template_files_container_name));

    if(keyval.empty()) {
        return;
    }

    std::set<std::string> includes = jsonListToSet(vars.at("searchPaths").at("files").at("include"));
    std::set<std::string> excludes = jsonListToSet(vars.at("searchPaths").at("files").at("exclude"));

    std::set<std::string> included_files = matchPaths(getPaths(path_to_init_template_to, path_to_init_template_to), 
        includes, excludes);

    std::string var_prefix = vars.at("variablePrefix");
    std::string var_suffix = vars.at("variableSuffix");

    replaceVariablesInAllFiles(path_to_init_template_to, included_files, keyval, var_prefix, var_suffix);

    includes = jsonListToSet(vars.at("searchPaths").at("filenames").at("include"));
    excludes = jsonListToSet(vars.at("searchPaths").at("filenames").at("exclude"));

    included_files = matchPaths(getPaths(path_to_init_template_to, path_to_init_template_to), 
        includes, excludes);

    replaceVariablesInAllFilenames(path_to_init_template_to, included_files, keyval, var_prefix, var_suffix);
}

void initTemplate(const std::string& template_dir, const std::string& template_name, const std::string& template_files_container_name, 
                const std::string& path_to_init_template_to, const std::unordered_map<std::string, std::string>& keyval)
{
    return initTemplate(path::joinPath(template_dir, template_name), template_files_container_name, path_to_init_template_to, keyval);
}

void addTemplate(const std::string& template_dir, const std::string& path_to_add, const std::string& name, const std::string& desc, const std::string& container_name)
{
    // If name is empty
    if(name.empty()) {
        std::cout << "[ERROR] Name cannot be empty" << std::endl;
        return;
    }

    // Check if there is an invalid filename character in the name
    for(const auto& i : name) {
        if(!path::isValidFilenameChar(i)) {
            std::cout << "[ERROR] Invalid filename character \"" << i << "\" in \"" << name << "\"" << std::endl;
            return;
        }
    }

    // Check if name already exists in available templates
    std::string new_template_path = path::joinPath(template_dir, name);
    if(path::exists(new_template_path)) {
        std::cout << "[ERROR] Template \"" << name << "\" already exists" << std::endl;
        return;
    }
    
    path::createDirectory(new_template_path);

    // Add directory separator to only copy the contents of the path and not its folder
    path::copy(path_to_add + path::directorySeparator(), new_template_path, path::CopyOption::None);

    std::string new_container_path = path::joinPath(new_template_path, container_name);

    // If a container already exists, delete it and replace it with a fresh one
    if(path::exists(new_container_path)) {
        path::remove(new_container_path);
    }

    path::createDirectory(new_container_path);

    json info = {
        {"description", desc}
    };
    json variables = json::parse(R"(
        {
            "searchPaths": {
                "files": {
                    "include": [],
                    "exclude": []
                },
                "filenames": []
            },
            "variablePrefix": "!",
            "variableSuffix": "!",
            "variableList": []
        }
    )");

    writeJsonToFile(info, path::joinPath(new_container_path, "info.json"), 4);
    writeJsonToFile(variables, path::joinPath(new_container_path, "variables.json"), 4);

    std::cout << "[SUCCESS] Template \"" << name << "\" has been initialized" << std::endl;
}

void removeTemplates(const std::string& template_dir, const std::vector<std::string>& templates)
{
    std::vector<std::string> deleted;
    for(int i = 0; i < templates.size(); i++) {
        std::string template_path = path::joinPath(template_dir, templates[i]);
        if(path::exists(template_path)) {
            path::remove(template_path);
            deleted.push_back(templates[i]);
        }
    }

    if(!deleted.empty()) {
        std::cout << "[SUCCESS] Templates ";
        for(int i = 0; i < deleted.size(); i++) {
            std::cout << "\"" << deleted[i] << "\"";
            if(i < deleted.size()-1) {
                std::cout << ", ";
            }
        }
        std::cout << " have been removed" << std::endl;
    }
}

void listTemplates(const std::string& template_dir, const std::string& container_name)
{
    // If the template directory is empty
    if(path::isEmpty(template_dir)) {
        std::cout << "[ERROR] No Templates Found" << std::endl;
        return;
    }

    std::vector<std::vector<std::string>> v; // A table
    std::cout << "Templates:" << std::endl;    

    // Iterate through the whole template directory
    for(const auto& i : std::filesystem::directory_iterator(template_dir)) {
        std::vector<std::string> temp; // Row for the table
        std::string template_name = i.path().filename().string();
        temp.push_back(template_name);
        std::string info_file = path::joinPath({i.path(), container_name, "info.json"});

        // Check if info.json exists
        if(!path::exists(info_file)) {
            v.push_back(temp);
            continue;
        }

        json info = readJsonFromFile(info_file);
        temp.push_back(info.at("description"));
        v.push_back(temp);
    }

    // Format the outputted text
    std::vector<std::string> output = getAlignedOutput(v, 40);
    for(const auto& i : output) {
        std::cout << "  " << i << std::endl;
    }
}