#include "cli.hpp"
#include "path.hpp"

void printHelp(const std::string& program, const std::string& subcmd)
{
    if(subcmd.empty()) {
        std::cout << "Usage:" << std::endl;
        std::cout << program << "<options> <values>" << std::endl;
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

void initTemplate()
{

}

int main(int argc, char* argv[])
{
    CLI cli(argc, argv);
    std::string program_name = cli.getProgramName();
    try {
        cli.setValidSubcommands({"init", "add", "remove", "rename", "edit"});
        cli.setValidFlags({"-h", "--help", "--version", "--set-template-directory", "--set-default-editor", "--config"});
        cli.setValidFlags("init", {"-h", "--help", "-l", "--list", "-s", "--skip-existing", 
        "-o", "--overwrite-existing", "-f", "--force", "-p", "--path"});
        cli.setValidFlags("add", {"-h", "--help", "-p", "--path"});
        cli.setValidFlags("remove", {"-h", "--help", "-l", "--list"});
        cli.setValidFlags("rename", {"-h", "--help", "-l", "--list"});
        cli.setValidFlags("edit", {"-h", "--help", "-l", "--list"});

        std::string subcmd = cli.getActiveSubcommand();

        if(argc == 1 || cli.isFlagActive({"-h", "--help"})) {
            printHelp(program_name, subcmd);
        }

    } catch(const CLIException& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}