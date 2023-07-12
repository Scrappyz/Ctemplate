#include "cli.hpp"
#include "path.hpp"
#include "config.hpp"

void printHelp(const std::string& program, const std::string& subcmd)
{
    if(subcmd.empty()) {
        std::cout << "Usage:" << std::endl;
        std::cout << program << " <options> <values>" << std::endl;
        std::cout << program << " <subcommand> <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Subcommands: " << std::endl;
        std::cout << "init                             Initializes a template project" << std::endl;
        std::cout << "remove                           Remove an existing template" << std::endl;
        std::cout << "rename                           Rename an existing template" << std::endl;
        std::cout << "edit                             Edit an existing template" << std::endl;
        std::cout << std::endl;
        std::cout << "Options: " << std::endl;
        std::cout << "-h, --help                       Print help menu" << std::endl;
        std::cout << "--version                        Print program version" << std::endl;
        std::cout << "--set-template-directory         Set the directory to look for templates" << std::endl; 
        std::cout << "--set-default-editor             Set the default editor" << std::endl;
        std::cout << "--set-config-path                Set the path of the config path" << std::endl;
        std::cout << "--config                         Open config file" << std::endl;
    } else if(subcmd == "init") {
        std::cout << "Usage:" << std::endl;
        std::cout << program << " " << subcmd << " <template-name> <options> [-p <path>]" << std::endl;
        std::cout << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "-h, --help                       Print help menu" << std::endl;
        std::cout << "-l, --list                       Print all existing templates" << std::endl;
        std::cout << "-p, --path                       The path the template will be initialized on (default is the current path)" << std::endl;
        std::cout << "-s, --skip-existing              Skip all existing files" << std::endl; 
        std::cout << "-o, --overwrite-existing         Overwrites all existing files" << std::endl;
        std::cout << "-f, --force                      Overwrites the whole directory" << std::endl;
    } else if(subcmd == "add") {
        std::cout << "Usage:" << std::endl;
        std::cout << program << " " << subcmd << " <new-template-name> [-p <path>]" << std::endl;
        std::cout << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "-h, --help                       Print help menu" << std::endl;
        std::cout << "-p, --path                       The directory to add as a template (default is the current directory)" << std::endl;
    } else if(subcmd == "remove") {
        std::cout << "Usage:" << std::endl;
        std::cout << program << " " << subcmd << " <existing-template-name>" << std::endl;
        std::cout << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "-h, --help                       Print help menu" << std::endl;
        std::cout << "-l, --list                       Print all existing templates" << std::endl;
    } else if(subcmd == "rename") {
        std::cout << "Usage:" << std::endl;
        std::cout << program << " " << subcmd << " <existing-template-name> <new-template-name>" << std::endl;
        std::cout << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "-h, --help                       Print help menu" << std::endl;
        std::cout << "-l, --list                       Print all existing templates" << std::endl;
    } else if(subcmd == "edit") {
        std::cout << "Usage:" << std::endl;
        std::cout << program << " " << subcmd << " <existing-template-name>" << std::endl;
        std::cout << program << " " << subcmd << " <options>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "-h, --help                       Print help menu" << std::endl;
        std::cout << "-l, --list                       Print all existing templates" << std::endl;
    } else {
        throw CLIException("[Error] " + subcmd + " is not a valid subcommand");
    }
}

std::string getConfigPath(const CLI& cli)
{
    std::string config_path = path::join(path::sourcePath(), "config.txt");
    if(path::exists(config_path)) {
        return config_path;
    }

    if(cli.noActiveSubcommand() && cli.isFlagActive("--set-config-path")) {
        std::string new_config_path = cli.getValueOf("--set-config-path");
        if(path::exists(new_config_path) && path::isFile(new_config_path)) {
            path::copy(new_config_path, path::sourcePath(), path::CopyOption::OverwriteExisting);
            return config_path;
        }
    }
    throw std::runtime_error("[Error] Could not find config file");
}

void changeConfigValue(const std::string& config_path, const std::string& section, const std::string& key, const std::string& new_val)
{
    std::string new_config = path::join(path::sourcePath(), "config_temp.txt");
    std::ifstream input(config_path);
    std::ofstream output(new_config);

    std::string temp;
    if(input.is_open() && output.is_open()) {
        std::string current_section;
        while(getline(input, temp)) {
            int i = 0;
            while(i < temp.size() && temp[i] == ' ') {
                i++;
            }

            if(i >= temp.size() || temp[i] == '#') {
                continue;
            }

            if(temp[i] == '[') {
                current_section.clear();
                i++;
                while(i < temp.size() && temp[i] != ']') {
                    current_section.push_back(temp[i]);
                    i++;
                }
                output << "[" << current_section << "]" << std::endl;
                continue;
            }

            if(current_section != section) {
                continue;
            }

            size_t f = temp.find(key);
            if(f != std::string::npos) {
                output << key << " = \"" << new_val << "\"" << std::endl;
            } else {
                output << temp << std::endl;
            }
        }

        input.close();
        output.close();

        path::remove(config_path);
        path::rename(new_config, "config.txt");
    } else {
        throw std::runtime_error("[Error] Could not open file");
    }
}

void changeConfigValue(const std::string& config_path, const std::string& key, const std::string& new_val)
{
    changeConfigValue(config_path, "", key, new_val);
}

void setAll(CLI& cli)
{
    cli.setValidSubcommands({"init", "add", "remove", "rename", "edit"});
    cli.setValidFlags({"-h", "--help", "--version", "--set-template-directory", "--set-default-editor", 
    "--set-config-path", "--config"});
    cli.setValidFlags("init", {"-h", "--help", "-l", "--list", "-s", "--skip-existing", 
    "-o", "--overwrite-existing", "-f", "--force", "-p", "--path"});
    cli.setValidFlags("add", {"-h", "--help", "-p", "--path"});
    cli.setValidFlags("remove", {"-h", "--help", "-l", "--list"});
    cli.setValidFlags("rename", {"-h", "--help", "-l", "--list"});
    cli.setValidFlags("edit", {"-h", "--help", "-l", "--list"});
}

int main(int argc, char* argv[])
{
    CLI cli(argc, argv);
    //cli.setArguments({"template", "--set-default-editor", "notepad"});
    Config config;
    std::string program_name = cli.getProgramName();
    try {
        setAll(cli);
        std::string subcmd = cli.getActiveSubcommand();

        if(argc == 1 || cli.isFlagActive({"-h", "--help"})) {
            printHelp(program_name, subcmd);
        }

        std::string config_path = getConfigPath(cli);

        if(subcmd.empty()) {
            if(cli.isFlagActive("--set-template-directory")) {
                changeConfigValue(config_path, "Templates", "directory", cli.getValueOf("--set-template-directory"));
            }
            if(cli.isFlagActive("--set-default-editor")) {
                changeConfigValue(config_path, "Templates", "editor", cli.getValueOf("--set-default-editor"));
            }
            if(cli.isFlagActive("--config")) {
                path::open(config_path);
            }
        }

        config.setConfig(config_path);

    } catch(const CLIException& e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch(const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}