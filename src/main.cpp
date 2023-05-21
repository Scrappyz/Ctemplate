#include <iostream>
#include <vector>
#include <fstream>
#include "path.hpp"

using namespace std;

void print(const vector<pair<string, int>>& v)
{
    for(int i = 0; i < v.size(); i++) {
        cout << "[" << v[i].first << " | " << v[i].second << "]" << endl;
    }
}

void copyDirectory(const string& source, const string& destination)
{
    if(pathExists(source) && isDirectory(source)) {
        vector<string> paths;
        int prev_lvl = 0;
        filesystem::recursive_directory_iterator iter(source);
        for(const auto& i : iter) {
            string current = i.path().filename().string();
            if(prev_lvl < iter.depth()) {
                paths.push_back(i.path().filename().string());
            } else if(prev_lvl > iter.depth()) {
                for(int j = prev_lvl - iter.depth(); j >= 0; j--) {
                    paths.pop_back();
                }
                paths.push_back(i.path().filename().string());
            } else {
                if(paths.empty()) {
                    paths.push_back(current);
                } else {
                    paths.back() = current;
                }
            }
            prev_lvl = iter.depth();
            //iter++;
        }
    } else {
        cerr << "[Error] Path does not exist" << endl;
    }
}

void test(const string& path)
{
    filesystem::recursive_directory_iterator iter(path);
    for(const auto& i : filesystem::recursive_directory_iterator(path)) {
        cout << "Depth: " << iter.depth() << endl;
        cout << i.path().string() << endl;
        cout << endl;
        iter++;
    }
}

int main(int argc, char** argv)
{
    vector<string> args = {"template1"};
    //args.assign(argv+1, argv+argc);
    string program_name = argv[0];

    if(args.empty()) {
        return 0;
    }
    
    string template_name = args[0];
    cout << joinPath(getSourcePath(), "Templates/" + template_name) << endl;
    cout << getCurrentPath() << endl;
    copyDirectory(joinPath(getSourcePath(), "Templates/" + template_name), getCurrentPath());
    //test(joinPath(getCurrentPath(), args[0]));
    return 0;
}