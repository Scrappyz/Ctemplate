#include "main.hpp"
#include "CLI11.hpp"
#include "helper.hpp"

using json = nlohmann::json;
namespace path = os::path;

void listTemplates()
{
    std::cout << "list templates" << std::endl;
}

int add(int a, int b)
{
    return a+b;
}

int main(int argc, char** argv)
{
    CLI::App app("Ctemplate");
    app.set_version_flag("-v,--version", "v1.0.0");

    json app_config = {
        {"templateDir", path::joinPath(path::sourcePath(), "templates")}
    };

    std::string config_file_path = path::joinPath(path::sourcePath(), "config.json");

    if(path::exists(config_file_path)) {
        app_config = readJsonFromFile(config_file_path);
    } else {
        writeJsonToFile(app_config, config_file_path, 4);
    }

    std::cout << app_config.at("templateDir") << std::endl;
    std::vector<std::string> s = split("key=val", "=");
    for(const auto& i : s) {
        std::cout << i << std::endl;
    }

    // bool list_template = false;

    // CLI::App* init = app.add_subcommand("init", "Initialize a template");
    // init->add_flag("-l,--list", list_template, "List existing templates");
    // std::string init_template_name;
    // init->add_option("name", init_template_name, "Name of the template to initialize");
    
    // CLI::App* add = app.add_subcommand("add", "Add a new template");
    // std::string add_path;
    // add->add_option("path", add_path, "Root project directory of template to be added")->expected(1);
    // std::string add_template_name;
    // add->add_option("-n,--name", add_template_name, "Name of new template")->expected(1);
    // std::string add_template_desc;
    // add->add_option("-d,--desc", add_template_desc, "Description of new template")->expected(1);

    // CLI::App* remove = app.add_subcommand("remove", "Remove an existing template");
    // std::vector<std::string> remove_template_names;
    // remove->add_option("names", remove_template_names, "Name of template to be removed");

    // CLI::App* list = app.add_subcommand("list", "List all templates");

    // CLI::App* config = app.add_subcommand("config", "Show config");
    // CLI::App* set = config->add_subcommand("set", "Set config values");
    // std::vector<std::string> config_set_values;
    // set->add_option("keyvalue", config_set_values, "Config values to change (eg: \"key=value\")");

    // CLI11_PARSE(app, argc, argv);

    // if(*init) {
    //     std::cout << "init" << std::endl;
    //     if(list_template) {
    //         listTemplates();
    //     }
    // } else if(*add) {
    //     std::cout << add_path << std::endl;
    //     std::cout << add_template_name << std::endl;
    //     std::cout << add_template_desc << std::endl;
    // } else if(*remove) {
    //     for(const auto& i : remove_template_names) {
    //         std::cout << i << std::endl;
    //     }
    // } else if(*list) {
    //     listTemplates();
    // } else {
    //     std::cout << "nuh uh" << std::endl;
    // }

    return 0;
}