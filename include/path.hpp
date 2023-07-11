#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <cstdlib>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <cstdlib>
#endif

namespace path {
    
    enum class CopyOption {None, SkipExisting, OverwriteExisting, OverwriteAll};
    enum class Traversal {NonRecursive, Recursive};
    enum class SizeMetric {Byte, Kilobyte, Megabyte, Gigabyte};

    namespace _private { // forward declaration
        char copyWarning(const std::filesystem::path& path);
        void copy(std::filesystem::path from, std::filesystem::path to, bool move, const CopyOption& op);
        bool execute(const char* command, bool wait);
    }

    inline bool exists(const std::filesystem::path& path)
    {
        return std::filesystem::exists(path);
    }

    inline bool isAbsolute(const std::filesystem::path& path)
    {
        return path.is_absolute();
    }

    inline bool isRelative(const std::filesystem::path& path)
    {
        return path.is_relative();
    }

    inline bool isValidFilenameChar(char ch) // checks if character passed is a valid character for filenames
    {
        switch(ch) {
        #if defined(_WIN32)
            case '<':
            case '>':
            case ':':
            case '\"':
            case '/':
            case '\\':
            case '|':
            case '?':
            case '*':
        #else
            case '/':
        #endif
                return false;
            default:
                return true;
        }
    }

    inline bool isDirectorySeparator(char ch) 
    {
        char preferred = std::filesystem::path::preferred_separator;
        return ch == preferred || ch == '/' && preferred == '\\';
    }

    inline std::string fileExtension(const std::filesystem::path& path)
    {
        std::string temp = path.filename().empty() ? path.parent_path().filename().string() : path.filename().string();
        int i = temp.size()-1;
        while(i > 0 && (temp[i] == ' ' || isDirectorySeparator(temp[i]))) {
            i--;
        }
        while(i > 0 && temp[i] != '.' && !isDirectorySeparator(temp[i])) {
            i--;
        }

        std::string result;
        if(i >= 1 && i < temp.size()-1 && temp[i] == '.' && isValidFilenameChar(temp[i-1])) {
            for(int j = i+1; j < temp.size() && isValidFilenameChar(temp[j]); j++) {
                result.push_back(temp[j]);
            }
        }
        return result;
    }

    inline bool hasFileExtension(const std::filesystem::path& path)
    {
        return !fileExtension(path).empty();
    }

    inline bool isDirectory(const std::filesystem::path& path)
    {
        return std::filesystem::is_directory(path);
    }

    inline bool isFile(const std::filesystem::path& path)
    {
        return std::filesystem::is_regular_file(path);
    }

    inline std::string filename(const std::filesystem::path& path) 
    {
        return path.filename().empty() ? path.parent_path().filename().string() : path.filename().string();
    }

    inline double size(const std::filesystem::path& path, const SizeMetric& metric = SizeMetric::Byte)
    {
        if(std::filesystem::exists(path)) {
            std::uintmax_t space = 0;
            if(std::filesystem::is_directory(path)) {
                for(const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                    if(!std::filesystem::is_directory(entry.path())) {
                        space += std::filesystem::file_size(entry.path());
                    }
                }
            } else {
                space = std::filesystem::file_size(path);
            }

            if(metric == SizeMetric::Kilobyte) {
                return (double)space / 1024;
            } else if(metric == SizeMetric::Megabyte) {
                return (double)space / (1024*1024);
            } else if(metric == SizeMetric::Gigabyte) {
                return (double)space / (1024*1024*1024);
            } else {
                return (double)space;
            }
        } else {
            return -1;
        }
    }

    inline char directorySeparator() 
    {
        return std::filesystem::path::preferred_separator;
    }

    inline std::string absolute(const std::filesystem::path& path)
    {
        return std::filesystem::absolute(path).string();
    }

    inline std::string relative(const std::filesystem::path& path, const std::filesystem::path& base_path = std::filesystem::current_path())
    {
        return std::filesystem::relative(path, base_path).string();
    }

    inline std::string join(const std::filesystem::path& p1, const std::filesystem::path& p2)
    {
        std::string result;
        
        if(p2.empty()) {
            result = std::filesystem::weakly_canonical(p1).string();
            bool exist = std::filesystem::exists(result);
            if(exist && p1.filename().empty()) {
                result.push_back(directorySeparator());
            } else if(!exist && (p1.filename() == "." || p1.filename() == "..")) {
                result.pop_back();
            }
            return result;
        }

        result = std::filesystem::weakly_canonical(p1 / p2).string();
        bool exist = std::filesystem::exists(result);
        if(exist && p2.filename().empty()) {
            result.push_back(directorySeparator());
        } else if(!exist && (p2.filename() == "." || p2.filename() == "..")) {
            result.pop_back();
        }
        return result;
    }

    inline std::string join(const std::vector<std::filesystem::path>& paths)
    {
        if(paths.empty()) {
            return std::string();
        } else if(paths.size() < 2) {
            return join(paths[0], "");
        }

        std::filesystem::path result = paths[0];
        for(int i = 1; i < paths.size()-1; i++) {
            result /= paths[i];
        }
        return join(result, paths.back());
    }

    inline std::string currentPath() 
    {
        return std::filesystem::current_path().string();
    }

    inline std::string sourcePath() 
    {
        std::filesystem::path source_path;
        #if defined(_WIN32)
            char path[MAX_PATH];
            GetModuleFileName(NULL, path, MAX_PATH);
            source_path = path;
        #elif defined(__linux__) || defined(__apple__)
            source_path = filesystem::canonical("/proc/self/exe");
        #else
            throw std::runtime_error("[Error][sourcePath] Unknown Operating System");
        #endif
        return source_path.parent_path().string();
    }

    inline std::string rootName(const std::filesystem::path& path)
    {
        return path.root_name().string();
    }

    inline void create(const std::filesystem::path& path, const std::string& data = "")
    {
        if(!std::filesystem::exists(path)) {
            if(hasFileExtension(path)) {
                std::ofstream file(path);
                file << data;
                file.close();
            } else {
                std::filesystem::create_directories(path);
            }
        } else if(std::filesystem::is_regular_file(path)) {
            char ch = _private::copyWarning(path.filename());
            if(ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                std::ofstream file(path);
                file << data;
                file.close();
            }
        }
    }

    inline void create(const std::filesystem::path& path, const std::vector<std::string>& data)
    {
        std::string str;
        for(int i = 0; i < data.size(); i++) {
            str += data[i];
            if(i < data.size()-1) {
                str.push_back('\n');
            }
        }
        create(path, str);
    }

    inline void rename(const std::filesystem::path& path, const std::string& new_name)
    {
        std::filesystem::rename(path, path.parent_path() / new_name);
    }

    inline void copy(const std::filesystem::path& from, const std::filesystem::path& to, const CopyOption& op = CopyOption::None)
    {
        _private::copy(from, to, false, op);
    }

    inline void move(const std::filesystem::path& from, const std::filesystem::path& to, const CopyOption& op = CopyOption::None)
    {
        _private::copy(from, to, true, op);
    }

    inline void remove(const std::filesystem::path& path)
    {
        if(std::filesystem::exists(path)) {
            if(std::filesystem::is_directory(path)) {
                std::filesystem::remove_all(path);
                if(path.filename().empty()) {
                    std::filesystem::create_directories(path);
                }
            } else {
                std::filesystem::remove(path);
            }
        }
    }

    inline std::string find(const std::filesystem::path& search_path, const std::string& file_to_find, int max_depth)
    {
        if(std::filesystem::exists(search_path)) {
            for(auto i = std::filesystem::recursive_directory_iterator(search_path); i != std::filesystem::recursive_directory_iterator(); i++) {
                if(max_depth >= 0 && i.depth() >= max_depth) {
                    i.disable_recursion_pending();
                } 
                if(i->path().filename() == file_to_find) {
                    return i->path().string();
                }
            }
            return std::string();
        } else {
            throw std::runtime_error("[Error][find] Path does not exists");
        }
    }

    inline std::string find(const std::filesystem::path& search_path, const std::string& file_to_find, const Traversal& pt = Traversal::NonRecursive)
    {
        int n = pt == Traversal::NonRecursive ? 0 : -1;
        return path::find(search_path, file_to_find, n);
    }

    inline std::vector<std::string> findAll(const std::filesystem::path& search_path, const std::string& file_to_find, int max_depth)
    {
        std::vector<std::string> matches;
        if(std::filesystem::exists(search_path)) {
            for(auto i = std::filesystem::recursive_directory_iterator(search_path); i != std::filesystem::recursive_directory_iterator(); i++) {
                if(max_depth >= 0 && i.depth() >= max_depth) {
                    i.disable_recursion_pending();
                } 
                if(i->path().filename() == file_to_find) {
                    matches.push_back(i->path().string());
                }
            }
            return matches;
        } else {
            throw std::runtime_error("[Error][find] Path does not exists");
        }
    }

    inline std::vector<std::string> findAll(const std::filesystem::path& search_path, const std::string& file_to_find, const Traversal& pt = Traversal::NonRecursive)
    {
        int n = pt == Traversal::NonRecursive ? 0 : -1;
        return path::findAll(search_path, file_to_find, n);
    }

    inline bool execute(const std::string& command, bool wait = false)
    {
        return _private::execute(command.c_str(), wait);
    }

    inline bool execute(const std::vector<std::string>& command, bool wait = false)
    {
        std::string cstr;
        for(int i = 0; i < command.size(); i++) {
            cstr.append('"' + command[i] + '"');
            if(i < command.size()-1) {
                cstr.push_back(' ');
            }
        }
        return _private::execute(cstr.c_str(), wait);
    }

    inline bool execute(const std::initializer_list<std::string>& command, bool wait = false)
    {
        return execute(std::vector<std::string>(command), wait);
    }

    inline bool open(const std::filesystem::path& file, const std::filesystem::path& program = "")
    {
        bool success = false;

        #if defined(_WIN32)
            if(program.empty()) {
                HINSTANCE result = ShellExecuteW(NULL, L"open", file.c_str(), NULL, NULL, SW_SHOWNORMAL);
                success = (INT_PTR)result > 32;
            } else {
                HINSTANCE result = ShellExecuteW(NULL, L"open", program.c_str(), file.c_str(), NULL, SW_SHOWNORMAL);
                success = (INT_PTR)result > 32;
            }
        #elif defined(__APPLE__) || defined(__linux__)
            std::string command = "xdg-open";
            if (!program.empty()) {
                command = program.string();
            }

            std::string filePath = file.string();

            int result = std::system((command + " \"" + filePath + "\"").c_str());
            success = (result == 0);
        #endif

        return success;
    }

    namespace _private {

        inline char copyWarning(const std::filesystem::path& path)
        {
            char ch;
            std::cout << "[Warning] \"" << path.string() << "\" already exists. Would you like to overwrite?" << std::endl;
            std::cout << "[Y] for yes, [N] for no, [A] for yes to all, [X] to cancel: ";
            std::cin >> ch;
            std::cin.clear();
            std::cin.ignore(256, '\n');
            return ch; 
        }

        inline bool copyFile(const std::filesystem::path& from, const std::filesystem::path& to) 
        {
            std::ifstream source(from, std::ios::binary);
            if(!source.is_open()) {
                return false;
            }

            std::ofstream destination(to, std::ios::binary);
            if(!destination.is_open()) {
                source.close();
                return false;
            }

            destination << source.rdbuf(); 

            if(!destination) {
                source.close();
                destination.close();
                return false;
            }

            source.close();
            destination.close();

            return true;
        }

        inline void copy(std::filesystem::path from, std::filesystem::path to, bool move, const CopyOption& op)
        {
            if(std::filesystem::exists(from)) {
                char ch;
                if(std::filesystem::is_directory(from)) {
                    if(!std::filesystem::is_directory(to)) {
                        throw std::runtime_error("[Error][copy] \"" + to.filename().string() + "\" is a file");
                    }

                    if(op == CopyOption::OverwriteAll) {
                        path::remove(to);
                        std::filesystem::create_directories(to);
                    } 
                    
                    if(!from.filename().empty()) {
                        to = std::filesystem::weakly_canonical(to / from.filename());
                        std::filesystem::create_directories(to);
                    }

                    for(const auto& entry : std::filesystem::recursive_directory_iterator(from)) {
                        std::filesystem::path copy_to = to / std::filesystem::relative(entry.path(), from);
                        bool is_source_dir = std::filesystem::is_directory(entry.path());
                        bool destination_exists = std::filesystem::exists(copy_to);
                        if(op == CopyOption::None && destination_exists && ch != 'a' && ch != 'A') {
                            ch = _private::copyWarning(path::relative(copy_to));
                        }

                        if(ch == 'x' || ch == 'X') {
                            return;
                        }

                        if(is_source_dir) { 
                            std::filesystem::create_directories(copy_to);
                        } else if(!destination_exists || op == CopyOption::OverwriteExisting || ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                            _private::copyFile(entry.path(), copy_to);
                        } 
                    }
                } else {
                    if(from.filename().empty()) {
                        from = from.parent_path();
                    }

                    bool is_source_dir = std::filesystem::is_directory(from);
                    bool is_destination_dir = std::filesystem::is_directory(to);

                    if(is_destination_dir && op == CopyOption::OverwriteAll) {
                        path::remove(to);
                        std::filesystem::create_directories(to);
                    } 

                    std::filesystem::path copy_to = std::filesystem::is_directory(to) ? std::filesystem::weakly_canonical(to / path::filename(from)) : to;
                    bool destination_exists = std::filesystem::exists(copy_to);

                    if(op == CopyOption::None && destination_exists && ch != 'a' && ch != 'A') {
                        ch = _private::copyWarning(path::relative(copy_to));
                    }

                    if(ch == 'x' || ch == 'X') {
                        return;
                    }

                    if(is_source_dir) { 
                        std::filesystem::create_directories(copy_to);
                    } else if(!destination_exists || op == CopyOption::OverwriteExisting || ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                        _private::copyFile(from, copy_to);
                    } 
                }
            } else {
                throw std::runtime_error("[Error][copy] \"" + from.string() + "\" does not exist");
            }

            if(move) {
                path::remove(from);
            }
        }

        inline bool execute(const char* command, bool wait)
        {
            #if defined(_WIN32)
                STARTUPINFO si;
                PROCESS_INFORMATION pi;
                ZeroMemory(&si, sizeof(si));
                ZeroMemory(&pi, sizeof(pi));
                si.cb = sizeof(si);

                if(CreateProcess(NULL, const_cast<LPSTR>(command), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    if(wait) {
                        WaitForSingleObject(pi.hProcess, INFINITE);
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);
                    }
                    return true;
                } 

                return false;
            #else
                pid_t pid = fork();

                if(pid == 0) {
                    execl("/bin/sh", "sh", "-c", command, NULL);
                    _exit(1); 
                } else if(pid > 0) {
                    if(wait) {
                        wait(NULL); 
                    }
                    return true;
                } else {
                    return false;
                }
            #endif
        }

    }
}