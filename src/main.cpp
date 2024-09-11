#include "os.hpp"
#include "CLI11.hpp"
#include "gitupdate.hpp"
#include "helper.hpp"
#include "ctemplate.hpp"
#include "global.hpp"
#include <unordered_set>

using json = nlohmann::json;
namespace path = os::path;

// Remove on release
void print(const std::vector<std::string>& v)
{
    for(const auto& i : v) {
        std::cout << i << std::endl;
    }
}

int update(const std::string& current_version, std::string tag, const std::string& asset_name, bool allow_pre_release)
{
    if(current_version == tag) {
        std::cout << "[SUCCESS] Software is up to date." << std::endl;
        return 0;
    }
    
    if(!gitupdate::isCurlInstalled()) {
        std::cout << "[ERROR] cURL not found." << std::endl;
        std::cout << "        Install cURL or update manually in " << global::github_url << std::endl;
        return 1;
    }

    std::cout << "[INFO] Checking for updates..." << std::endl;
    json release_info = tag.empty() ? gitupdate::getLatestReleaseJson(global::github_url, allow_pre_release)
                                    : gitupdate::getReleaseJson(global::github_url, tag);

    if(release_info.empty()) {
        std::cout << "[ERROR] Could not get release info." << std::endl;
        return 1;
    }

    tag = release_info.at("tag_name");

    if(current_version == tag) {
        std::cout << "[SUCCESS] Software is up to date." << std::endl;
        return 0;
    }

    if(gitupdate::updateApp(release_info, asset_name)) {
        std::cout << "[SUCCESS] Updated to " << tag << std::endl;
    } else {
        std::cout << "[ERROR] Update not found." << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    CLI::App app("Ctemplate");
    app.set_version_flag("-v,--version", global::app_version);

    // Default settings
    json app_config = global::app_config;

    std::string config_file_path = path::joinPath(path::sourcePath(), "config.json");

    // if config file exists, read from it. Else, use default settings then create the config file.
    if(path::exists(config_file_path)) {
        app_config = helper::readJsonFromFile(config_file_path);
    } else {
        helper::writeJsonToFile(app_config, config_file_path, 4);
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

    // For main command
    bool list_template = false;
    std::string tag;
    bool allow_pre_release = false;
    CLI::Option* update_opt = app.add_option("-U,--update", tag, "Update application to new version.\n(Updates to latest if no tag is specified)")->expected(0, 1);
    app.add_flag("-p", allow_pre_release, "Allow pre-release versions to be installed.")->needs(update_opt);

    // For "init" subcommand
    CLI::App* init = app.add_subcommand("init", "Initialize a template");
    std::string init_template_name;
    std::string init_path;
    bool init_force_overwrite = false;
    std::vector<std::string> init_keyval;
    std::vector<std::string> init_includes = {"**"};
    std::vector<std::string> init_excludes;
    init->add_option("name", init_template_name, "Name of the template to initialize.\nUse the 'list' subcommand to see available templates")->required();
    init->add_option("-p, --path", init_path, "Path to initialize the template to\n(defaults to the current path)")->expected(1);
    init->add_flag("-f,--force", init_force_overwrite, "Overwrites the entire directory\nwith the template");
    init->add_option("-v, --variables", init_keyval, "Set variable values.\n(E.g: projectName=\"Hello World\")\nUse the 'info' subcommand to see the variables of a template");
    init->add_option("-i,--include", init_includes, "Paths to include in the\ntemplate when initializing\n(E.g: \"project/main.py\")");
    init->add_option("-e,--exclude", init_excludes, "Paths to exclude in the\ntemplate when initializing\n(E.g: \"project/main.py\")");
    
    // For "add" subcommand
    CLI::App* add = app.add_subcommand("add", "Add a new template");
    std::string add_path;
    add->add_option("path", add_path, "Root project directory of\nthe template to be added\n(defaults to the current path)")->expected(1);
    std::string add_template_name;
    add->add_option("-n,--name", add_template_name, "Name of the new template")->expected(1)->required();
    std::string add_template_author;
    add->add_option("-a,--author", add_template_author, "Author of the new template")->expected(0, 1);
    std::string add_template_desc;
    add->add_option("-d,--desc", add_template_desc, "Description of the new template")->expected(0, 1);

    // For "remove" subcommand
    CLI::App* remove = app.add_subcommand("remove", "Remove an existing template");
    std::vector<std::string> remove_template_names;
    remove->add_option("names", remove_template_names, "Name of the template to be removed");

    // For "list" subcommand
    CLI::App* list = app.add_subcommand("list", "List all templates");

    // For "info" subcommand
    CLI::App* info = app.add_subcommand("info", "Show info about a template");
    std::string info_template;
    info->add_option("template", info_template, "Template to get info from.\nUse the 'list' subcommand to see available templates")->required();

    // For "config" subcommand
    CLI::App* config = app.add_subcommand("config", "Show config");

    // For "config set" subcommand
    CLI::App* set = config->add_subcommand("set", "Set config values");
    std::vector<std::string> config_set_values;
    set->add_option("keyvalue", config_set_values, "Config values to change\n(eg: \"key=value\")");

    // For "config reset" subcommand
    CLI::App* reset = config->add_subcommand("reset", "Reset config");
    std::vector<std::string> config_reset_values;
    reset->add_option("template", config_reset_values, "Template to reset config");

    CLI11_PARSE(app, argc, argv);

    // print(init_includes);

    if(update_opt->count() > 0) {
        return update(global::app_version, tag, global::asset_name, allow_pre_release);
    }

    if(*init) { // "init" subcommand
        std::string init_to = path::joinPath(path::currentPath(), init_path);
        std::string template_path_to_init = path::joinPath(template_dir, init_template_name);
        std::set<std::string> paths = helper::matchPaths(helper::getPaths(template_path_to_init, template_path_to_init),
                                       helper::arrayToSet(init_includes), helper::arrayToSet(init_excludes));
        initTemplate(template_dir, init_template_name, paths, container_name, 
                     init_to, helper::mapKeyValues(init_keyval), init_force_overwrite);
    } else if(*add) { // "add" subcommand
        std::string path_to_add = path::joinPath(path::currentPath(), add_path);
        addTemplate(template_dir, path_to_add, add_template_name, add_template_author, add_template_desc, container_name);
    } else if(*remove) { // "remove" subcommand
        removeTemplates(template_dir, remove_template_names);
    } else if(*list) { // "list" subcommand
        listTemplates(template_dir, container_name);
    } else if(*info) { // "into" subcommand
        printTemplateInfo(template_dir, info_template, container_name);
    } else if(*config) { // "config" subcommand
        if(*set) { // "set" subcommand
            helper::setConfigValue(app_config, config_set_values);
            helper::writeJsonToFile(app_config, config_file_path, 4);
            return 0;
        } else if(*reset) { // "reset" subcommand
            if(config_reset_values.empty()) {
                helper::resetConfig(config_file_path);
            } else {
                helper::resetConfig(template_dir, container_name, config_reset_values);
            }
            return 0;
        }
        helper::showConfig(app_config);
    } else {
        CLI::CallForHelp();
    }

    return 0;
}