#include "main.hpp"
#include "util.hpp"
#include "cli.hpp"
#include "path.hpp"
#include "config.hpp"

void printHelp(const CLI& cli, const Config& config)
{
    std::string program = cli.getProgramName();
    std::string subcmd = cli.getActiveSubcommand();
    if(subcmd.empty()) {
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << program << " <options> <values>" << std::endl;
        std::cout << "  " << program << " <subcommand> <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Subcommands: " << std::endl;
        std::cout << "  init                             Initializes a template project" << std::endl;
        std::cout << "  add                              Add a new template" << std::endl;
        std::cout << "  remove                           Remove an existing template" << std::endl;
        std::cout << "  edit                             Edit an existing template" << std::endl;
        std::cout << std::endl;
        std::cout << "Options: " << std::endl;
        std::cout << "  -h, --help                       Print help menu" << std::endl;
        std::cout << "  -l, --list                       Print existing templates" << std::endl;
        std::cout << "  --version                        Print program version" << std::endl;
        std::cout << "  --set-template-dir               Set the directory to look for templates" << std::endl; 
        std::cout << "  --setup                          Setup the program" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Template Directory: \"" + config.getValue("template_directory") + "\"" << std::endl;
    } else if(subcmd == "init") {
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <template> <options> [-p <path>]" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help                       Print help menu" << std::endl;
        std::cout << "  -l, --list                       Print existing templates" << std::endl;
        std::cout << "  -p, --path                       The path the template will be initialized on (default is the current path)" << std::endl;
        std::cout << "  -s, --skip-existing              Skip all existing files" << std::endl; 
        std::cout << "  -o, --overwrite-existing         Overwrites all existing files" << std::endl;
        std::cout << "  -f, --force                      Overwrites the whole directory" << std::endl;
    } else if(subcmd == "add") {
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <new-template> [-p <path>]" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help                       Print help menu" << std::endl;
        std::cout << "  -p, --path                       The directory to add as a template (default is the current directory)" << std::endl;
        std::cout << "  -d, --desc                       Add a description" << std::endl;
    } else if(subcmd == "remove") {
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <existing-template>" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help                       Print help menu" << std::endl;
        std::cout << "  -l, --list                       Print existing templates" << std::endl;
    } else if(subcmd == "edit") {
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <options>" << std::endl;
        std::cout << "  " << program << " " << subcmd << " <existing-template> <options> <values>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help                       Print help menu" << std::endl;
        std::cout << "  -l, --list                       Print existing templates" << std::endl;
        std::cout << "  -d, --desc                       Edit the description of a template" << std::endl;
        std::cout << "  -r, --rename                     Rename the template" << std::endl;
    } else {
        throw CLIException("[Error] " + subcmd + " is not a valid subcommand");
    }
}

void printVersion(const std::string& program)
{
    std::cout << program << " v1.0.0" << std::endl;
}

void setup()
{
    Config config;
    std::string temp;

    std::cout << "=====SETUP MENU=====" << std::endl;
    std::cout << "The template directory is where the templates will be saved (eg: D:/Documents/My Templates)" << std::endl;
    std::cout << "Set template directory: ";
    getline(std::cin, temp);
    temp = trim(temp);
    if(temp.empty()) {
        temp = path::joinPath(path::sourcePath(), "Templates");
    } else {
        temp = path::joinPath(path::currentPath(), temp);
    }
    config.addKeyValue("template_directory", temp);
    config.saveConfigToFile(path::joinPath(path::sourcePath(), "config.txt"));

    std::cout << "=====SETUP COMPLETE=====" << std::endl;
}

void listTemplates(const Config& config)
{
    std::filesystem::path template_dir = getTemplateDirectory(config);

    if(!path::exists(template_dir)) {
        throw std::runtime_error("[Error] The template directory does not exist");
    }

    std::cout << "Templates:" << std::endl;
    for(const auto& entry : std::filesystem::directory_iterator(template_dir)) {
        std::string template_name = entry.path().filename().string();
        std::string info_file = path::joinPath(entry.path(), ".template");
        Config info;

        if(path::exists(info_file)) {
            info.setConfigFromFile(info_file);
        }

        std::cout << "  " << template_name;
        if(info.doesKeyExist("description")) {
            std::cout << getSpaces(35, template_name) << info.getValue("description");
        }
        std::cout << std::endl;
    }
}

void initTemplate(const CLI& cli, const Config& config)
{
    std::string template_name = cli.getAnyValue({"-p", "--path"});
    if(template_name.empty()) {
        throw CLIException("[Error] No template provided");
    }

    std::string template_path = path::joinPath(getTemplateDirectory(config), template_name);

    if(!path::exists(template_path)) {
        throw std::runtime_error("[Error] Template \"" + template_name + "\" does not exist");
    }

    path::CopyOption option = path::CopyOption::None;
    if(cli.isFlagActive({"-s", "--skip-existing"})) {
        option = path::CopyOption::SkipExisting;
    } else if(cli.isFlagActive({"-o", "--overwrite-existing"})) {
        option = path::CopyOption::OverwriteExisting;
    }

    std::string to = path::currentPath();
    if(cli.isFlagActive({"-p", "--path"})) {
        to = cli.getValueOf({"-p", "--path"});
    }

    if(cli.isFlagActive({"-f", "--force"})) {
        path::remove(to + '/');
    }
    
    for(const auto& entry : std::filesystem::directory_iterator(template_path)) {
        if(entry.path().filename() == ".template") {
            continue;
        }
        path::copy(entry.path(), to, option);
    }

    std::cout << "[Success] Template \"" + template_name + "\" has been initialized" << std::endl;
}

void addTemplate(const CLI& cli, const Config& config)
{
    std::string path_to_add;
    if(cli.isFlagActive({"-p", "--path"})) {
        path_to_add = cli.getValueOf({"-p", "--path"});
    }

    if(path_to_add.empty() || path_to_add == ".") {
        path_to_add = path::currentPath();
    }

    path_to_add.push_back('/');

    std::string template_name = cli.getValueOf();
    std::string template_path = path::joinPath(getTemplateDirectory(config), template_name);

    if(path::exists(template_path)) {
        char ch;
        std::cout << "[Warning] \"" + template_name + "\" already exists. Would you like to overwrite? [y/n]: ";
        std::cin >> ch;
        if(ch != 'y' && ch != 'Y') {
            return;
        }
    }

    path::copy(path_to_add, template_path, path::CopyOption::OverwriteAll);

    Config template_info;
    if(cli.isFlagActive({"-d", "--desc"})) {
        template_info.addKeyValue("description", cli.getValueOf({"-d", "--desc"}));
        template_info.saveConfigToFile(path::joinPath(template_path, ".template"));
    }

    std::cout << "[Success] Template \"" + template_name + "\" has been added" << std::endl;
}

void removeTemplate(const CLI& cli, const Config& config)
{
    std::vector<std::string> template_names = cli.getAllValuesOf();

    if(template_names.empty()) {
        throw std::runtime_error("[Error] No template provided");
    }

    std::string template_dir = getTemplateDirectory(config);
    std::vector<std::string> removed_templates;
    for(int i = 0; i < template_names.size(); i++) {
        std::string template_path = path::joinPath(template_dir, template_names[i]);

        if(!path::exists(template_path)) {
            continue;
        }

        path::remove(template_path);
        removed_templates.push_back(template_names[i]);
    }

    if(!removed_templates.empty()) {
        std::cout << "[Success] Template";
        if(removed_templates.size() > 1) {
            std::cout << "s";
        }
        std::cout << " ";
        for(int i = 0; i < removed_templates.size(); i++) {
            std::cout << "\"" << removed_templates[i] << "\"";
            if(i < removed_templates.size()-1) {
                std::cout << ", ";
            }
        }
        std::cout << " has been removed";
    }
}

void editTemplate(const CLI& cli, const Config& config)
{
    std::string template_name = cli.getValueOf();

    if(template_name.empty()) {
        throw std::runtime_error("[Error] No template provided");
    }

    std::string template_path = path::joinPath(getTemplateDirectory(config), template_name);
    if(!path::exists(template_path)) {
        throw std::runtime_error("[Error] Template \"" + template_name + "\" does not exist");
    }

    bool is_description = cli.isFlagActive({"-d", "--desc"});
    bool is_rename = cli.isFlagActive({"-r", "--rename"});

    if(is_description) {
        Config info;
        info.addKeyValue("description", cli.getValueOf({"-d", "--desc"}));
        info.saveConfigToFile(path::joinPath(template_path, ".template"));
        std::cout << "[Success] Description of \"" << template_name << "\" has been updated" << std::endl;
    }

    if(is_rename) {
        std::string new_name = cli.getValueOf({"-r", "--rename"});
        path::rename(template_path, new_name);
        std::cout << "[Success] Template \"" << template_name << "\" has been renamed to " << new_name << std::endl;
    }

    if(!is_rename && !is_description) {
        path::open(template_path);
    }
}

void setAll(CLI& cli)
{
    cli.addSubcommands({"init", "add", "remove", "edit"});
    cli.addGlobalFlags({"-h", "--help"});
    cli.addGlobalFlags({"-l", "--list"}, {"add"});
    cli.addFlags({"-v", "--version", "--setup", "--set-template-dir"});
    cli.addFlags("init", {"-l", "--list", "-s", "--skip-existing", 
    "-o", "--overwrite-existing", "-f", "--force", "-p", "--path"});
    cli.addFlags("add", {"-p", "--path", "-d", "--desc"});
    cli.addFlags("edit", {"-d", "--desc", "-r", "--rename"});

    cli.init();
}

int main(int argc, char* argv[])
{
    CLI cli(argc, argv);
    std::string program_name = cli.getProgramName();
    std::string config_path = path::joinPath(path::sourcePath(), "config.txt");
    Config config;
    try {
        config.addKey("template_directory");

        setAll(cli);
        std::string subcmd = cli.getActiveSubcommand();

        if(subcmd.empty() && cli.isFlagActive("--setup")) {
            setup();
            return 0;
        }

        config.setConfigFromFile(config_path);

        if(cli.isFlagActive({"-h", "--help"})) {
            printHelp(cli, config);
            return 0;
        }

        if(subcmd != "add" && cli.isFlagActive({"-l", "--list"})) {
            listTemplates(config);
            return 0;
        }

        if(subcmd.empty()) {
            if(cli.isFlagActive({"-v", "--version"})) {
                printVersion(program_name);
                return 0;
            }

            if(cli.isFlagActive("--set-template-dir")) {
                std::string value = cli.getValueOf("--set-template-dir");
                if(!path::isAbsolutePath(value)) {
                    value = path::joinPath(path::currentPath(), value);
                }
                config.modifyKeyValue("template_directory", value);
                config.saveConfigToFile(config_path);
            }
        } else if(subcmd == "init") {
            initTemplate(cli, config);
        } else if(subcmd == "add") {
            addTemplate(cli, config);
        } else if(subcmd == "remove") {
            removeTemplate(cli, config);
        } else if(subcmd == "edit") {
            editTemplate(cli, config);
        }
    } catch(const CLIException& e) {
        printHelp(cli, config);
        std::cout << std::endl;
        std::cout << e.what() << std::endl;
        return 1;
    } catch(const std::runtime_error& e) {
        printHelp(cli, config);
        std::cout << std::endl;
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}