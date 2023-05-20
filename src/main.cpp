#include <iostream>
#include <vector>
#include <fstream>
#include "path.hpp"

using namespace std;

void copyDirectory(const string& source, const string& destination)
{
    if(pathExists(source) && isDirectory(source)) {
        for(const auto& i : filesystem::directory_iterator(source)) {
            string source_path = i.path().string();
            string new_path = joinPath(destination, i.path().filename().string());
            if(isDirectory(source_path)) {
                filesystem::create_directories(new_path);
                copyDirectory(source_path, new_path);
            } else {
                ifstream src(source_path, ios::binary);
                ofstream dst(new_path, ios::binary);
                dst << src.rdbuf();
            }
        } 
    } else {
        cerr << "[Error] Path does not exist" << endl;
    }
}

int main(int argc, char** argv)
{
    vector<string> args = {"template1"};
    args.assign(argv+1, argv+argc);
    string program_name = argv[0];

    if(args.empty()) {
        return 0;
    }
    
    string template_name = args[0];
    cout << joinPath(getSourcePath(), "Templates/" + template_name) << endl;
    cout << getCurrentPath() << endl;
    copyDirectory(joinPath(getSourcePath(), "Templates/" + template_name), getCurrentPath());
    return 0;
}