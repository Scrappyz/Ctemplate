#include "os.hpp"
#include "CLI11.hpp"
#include "helper.hpp"
#include "ctemplate.hpp"
#include <unordered_set>

using json = nlohmann::json;
namespace path = os::path;

void print(const std::vector<std::string>& v)
{
    for(const auto& i : v) {
        std::cout << i << std::endl;
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
    std::string init_template_name;
    std::string init_path;
    bool init_force_overwrite = false;
    std::vector<std::string> init_keyval;
    std::vector<std::string> init_includes;
    std::vector<std::string> init_excludes;
    init->add_option("name", init_template_name, "Name of the template to initialize")->required();
    init->add_option("-p, --path", init_path, "Path to initialize to")->expected(1);
    init->add_flag("-f,--force", init_force_overwrite, "Force overwrite directory");
    init->add_option("-v, --variables", init_keyval, "Initialize variables");
    init->add_option("-i,--include", init_includes, "Include paths");
    init->add_option("-e,--exclude", init_excludes, "Exclude paths");
    
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

    // For "info" subcommand
    CLI::App* info = app.add_subcommand("info", "Info about templates");
    std::string info_template;
    info->add_option("template", info_template, "Template to get info from");

    CLI11_PARSE(app, argc, argv);

    print(init_includes);

    if(true) { // "init" subcommand
        std::string init_to = path::joinPath(path::currentPath(), init_path);
        std::string template_path_to_init = path::joinPath(template_dir, init_template_name);
        initTemplate(template_dir, init_template_name, 
                    matchPaths(getPaths(template_path_to_init, template_path_to_init), arrayToSet(init_includes), arrayToSet(init_excludes)), 
                    container_name, init_to, mapKeyValues(init_keyval), init_force_overwrite);
    } else if(*add) { // "add" subcommand
        std::string path_to_add = path::joinPath(path::currentPath(), add_path);
        addTemplate(template_dir, path_to_add, add_template_name, add_template_desc, container_name);
    } else if(*remove) { // "remove" subcommand
        removeTemplates(template_dir, remove_template_names);
    } else if(*list) { // "list" subcommand
        listTemplates(template_dir, container_name);
    } else if(*config) { // "config" subcommand
        if(*set) { // "set" subcommand
            setConfigValue(app_config, config_set_values);
            writeJsonToFile(app_config, config_file_path, 4);
            return 0;
        }
        std::cout << "Configuration:" << std::endl;
        showConfig(app_config, 2);
    } else if(*info) { // "into" subcommand
        printTemplateInfo(template_dir, info_template, container_name);
    } else {
        CLI::CallForHelp();
    }

    return 0;
}