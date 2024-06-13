#include <iostream>
#include <fstream>
#include "os.hpp"
#include "CLI11.hpp"
#include "helper.hpp"

using json = nlohmann::json;
namespace path = os::path;

void listTemplates(const std::string& template_dir, const std::string& container_name)
{
    // std::cout << "Templates:" << std::endl;
    // for(const auto& i : std::filesystem::directory_iterator(template_dir)) {
    //     std::string template_name = i.path().filename().string();
    //     std::cout << "  " << template_name;
    //     std::string info_file = path::joinPath({i.path(), container_name, "info.json"});
        
    //     if(!path::exists(info_file)) {
    //         std::cout << std::endl;
    //         continue;
    //     }

    //     json info = readJsonFromFile(info_file);
    // }
    std::vector<std::vector<std::string>> v = {{"wassup", "some description of\nmy balls"}, {"gago", "this is swear"}};
    std::vector<std::string> s = getAlignedOutput(v, 30);
    for(const auto& i : s) {
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
    add->add_option("-n,--name", add_template_name, "Name of new template")->expected(1);
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
        std::cout << add_path << std::endl;
        std::cout << add_template_name << std::endl;
        std::cout << add_template_desc << std::endl;
    } else if(*remove) {
        for(const auto& i : remove_template_names) {
            std::cout << i << std::endl;
        }
    } else if(*list) {
        listTemplates(template_dir, container_name);
    } else {
        std::cout << "nuh uh" << std::endl;
    }

    return 0;
}