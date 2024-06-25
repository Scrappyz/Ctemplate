#include <iostream>
#include <fstream>
#include "os.hpp"
#include "CLI11.hpp"
#include "helper.hpp"

using json = nlohmann::json;
namespace path = os::path;

void showConfig(const json& config, int space_before = 0)
{
    for(const auto& i : config.items()) {
        std::cout << std::string(space_before, ' ') << i.key() << " = " << i.value() << std::endl;
    }
}

void setConfigValue(json& config, const std::vector<std::string>& config_key_values)
{
    for(int i = 0; i < config_key_values.size(); i++) {
        std::vector<std::string> keyval = split(config_key_values[i], "=");

        if(!config.contains(keyval[0])) {
            std::cout << "[WARNING] Key \"" << keyval[0] << "\" does not exist" << std::endl;
            continue;
        }

        config[keyval[0]] = keyval[1];
    }
}

void addTemplate(const std::string& template_dir, const std::string& path_to_add, const std::string& name, const std::string& desc, const std::string& container_name)
{
    if(name.empty()) {
        std::cout << "[ERROR] Name cannot be empty" << std::endl;
        return;
    }

    for(const auto& i : name) {
        if(!path::isValidFilenameChar(i)) {
            std::cout << "[ERROR] Invalid filename character \"" << i << "\" in \"" << name << "\"" << std::endl;
            return;
        }
    }
    
    std::string new_template_path = path::joinPath(template_dir, name);
    if(!path::exists(new_template_path)) {
        path::createDirectory(new_template_path);
    }

    // Add directory separator to only copy the contents of the path and not its folder
    path::copy(path_to_add + path::directorySeparator(), new_template_path, path::CopyOption::None);

    std::string new_container_path = path::joinPath(new_template_path, container_name);

    if(path::exists(new_container_path)) {
        path::remove(new_container_path);
    }

    path::createDirectory(new_container_path);

    json info = {
        {"description", desc}
    };
    json variables = json::parse(R"(
        {
            "includePaths": [],
            "excludePaths": [],
            "variablePrefix": "!",
            "variableSuffix": "!",
            "variableList": []
        }
    )");

    writeJsonToFile(info, path::joinPath(new_container_path, "info.json"), 4);
    writeJsonToFile(variables, path::joinPath(new_container_path, "variables.json"), 4);
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

int main(int argc, char** argv)
{
    CLI::App app("Ctemplate");
    app.set_version_flag("-v,--version", "v1.0.0");

    // Default settings
    json app_config = {
        {"templateDirectory", path::joinPath(path::sourcePath(), "templates")},
        {"containerName", ".ctemplate"}
    };

    std::string config_file_path = path::joinPath(path::sourcePath(), "config.json");

    // if config file exists, read from it. Else, use default settings then create the config file.
    if(path::exists(config_file_path)) {
        app_config = readJsonFromFile(config_file_path);
    } else {
        writeJsonToFile(app_config, config_file_path, 4);
    }

    // Directory where templates are stored
    std::string template_dir = app_config.at("templateDirectory");

    // Check if absolute path or relative path. If relative, make it absolute, relative to source
    if(!path::isAbsolutePath(template_dir)) {
        template_dir = path::joinPath(path::sourcePath(), template_dir);
    }

    // If template directory does not exist, create one
    if(!path::exists(template_dir)) {
        path::createDirectory(template_dir);
    }

    std::string container_name = app_config.at("containerName");

    bool list_template = false;

    // For "init" subcommand
    CLI::App* init = app.add_subcommand("init", "Initialize a template");
    init->add_flag("-l,--list", list_template, "List existing templates");
    std::string init_template_name;
    init->add_option("name", init_template_name, "Name of the template to initialize");
    
    // For "add" subcommand
    CLI::App* add = app.add_subcommand("add", "Add a new template");
    std::string add_path;
    add->add_option("path", add_path, "Root project directory of template to be added")->expected(1);
    std::string add_template_name;
    add->add_option("-n,--name", add_template_name, "Name of new template")->expected(1)->required();
    std::string add_template_desc;
    add->add_option("-d,--desc", add_template_desc, "Description of new template")->expected(1);

    // For "remove" subcommand
    CLI::App* remove = app.add_subcommand("remove", "Remove an existing template");
    std::vector<std::string> remove_template_names;
    remove->add_option("names", remove_template_names, "Name of template to be removed");

    // For "list" subcommand
    CLI::App* list = app.add_subcommand("list", "List all templates");

    // For "config" subcommand
    CLI::App* config = app.add_subcommand("config", "Show config");

    // For "config set" subcommand
    CLI::App* set = config->add_subcommand("set", "Set config values");
    std::vector<std::string> config_set_values;
    set->add_option("keyvalue", config_set_values, "Config values to change (eg: \"key=value\")");

    CLI11_PARSE(app, argc, argv);

    if(*init) {
        std::cout << "init" << std::endl;
        if(list_template) {
            listTemplates(template_dir, container_name);
        }
    } else if(*add) {
        std::string path_to_add = path::joinPath(path::currentPath(), add_path);
        addTemplate(template_dir, path_to_add, add_template_name, add_template_desc, container_name);
    } else if(*remove) {
        removeTemplates(template_dir, remove_template_names);
    } else if(*list) {
        listTemplates(template_dir, container_name);
    } else if(*config) { // Config commands
        if(*set) { // Change config values
            setConfigValue(app_config, config_set_values);
            writeJsonToFile(app_config, config_file_path, 4);
            return 0;
        }
        std::cout << "Configuration:" << std::endl;
        showConfig(app_config, 2);
    } else {
        std::cout << "nuh uh" << std::endl;
    }

    return 0;
}