#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <set>
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

namespace os {
    // path namespace
    namespace path {
        
        enum class CopyOption {None, SkipExisting, OverwriteExisting, OverwriteAll};
        enum class TraversalOption {NonRecursive, Recursive};
        enum class SizeMetric {Byte, Kilobyte, Megabyte, Gigabyte};

        namespace _private { // forward declaration
            std::string errorMessage(const std::string& function_name, const std::string& message);
            char copyWarning(const std::filesystem::path& path);

            bool copy(const std::filesystem::path& source, const std::filesystem::path& destination, 
                      const CopyOption& op, const TraversalOption& t_op);

            bool copy(const std::filesystem::path& source, const std::set<std::string>& paths, 
                      const std::filesystem::path& destination, const CopyOption& op);

            bool move(const std::filesystem::path& source, const std::filesystem::path& destination, 
                      const CopyOption& op, const TraversalOption& t_op);

            bool move(const std::filesystem::path& source, const std::set<std::string>& paths, 
                      const std::filesystem::path& destination, const CopyOption& op);
        }

        inline bool exists(const std::filesystem::path& path)
        {
            return std::filesystem::exists(path);
        }

        inline bool isEmpty(const std::filesystem::path& path)
        {
            return std::filesystem::is_empty(path);
        }

        inline bool isAbsolutePath(const std::filesystem::path& path)
        {
            return path.is_absolute();
        }

        inline bool isRelativePath(const std::filesystem::path& path)
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

        inline bool isDirectorySeparator(char ch, bool any_separator = false) 
        {
            char preferred = std::filesystem::path::preferred_separator;

            if(any_separator) {
                return ch == '/' || ch == '\\';
            }

            return ch == preferred;
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

        inline std::string appendFileExtension(std::string path, const std::string& extension, bool force = false)
        {
            int i = 0;
            while(extension[i] == '.') {
                i++;
            }

            int extension_len = extension.size() - i;
            if(!force && path.size() > extension_len && path.substr(path.size()-extension_len) == extension.substr(i, extension_len)) {
                return path;
            }

            path.push_back('.');
            while(i < extension.size()) {
                path.push_back(extension[i]);
                i++;
            }

            return path;
        }

        inline bool hasFileExtension(const std::filesystem::path& path)
        {
            return !fileExtension(path).empty();
        }

        inline bool isDirectoryString(const std::filesystem::path& path)
        {
            return !path.empty() && path.filename().empty();
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

        inline std::string normalizePath(const std::filesystem::path& path)
        {
            std::string p = path.string();
            
            for(int i = 0; i < p.size(); i++) {
                if(isDirectorySeparator(p[i], true)) {
                    p[i] = directorySeparator();
                }
            }

            return p;
        }

        inline std::string absolutePath(const std::filesystem::path& path)
        {
            return std::filesystem::absolute(path).string();
        }

        inline std::string relativePath(const std::filesystem::path& path, const std::filesystem::path& base_path = std::filesystem::current_path())
        {
            return std::filesystem::relative(path, base_path).string();
        }

        inline std::string parentPath(std::filesystem::path path, int pop = 1)
        {
            for(int i = 1; i <= pop; i++) {
                path = path.parent_path();
            }

            return path.string();
        }

        inline std::string joinPath(const std::filesystem::path& p1, const std::filesystem::path& p2)
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

        inline std::string joinPath(const std::vector<std::filesystem::path>& paths)
        {
            if(paths.empty()) {
                return std::string();
            } else if(paths.size() < 2) {
                return joinPath(paths[0], "");
            }

            std::filesystem::path result = paths[0];
            for(int i = 1; i < paths.size()-1; i++) {
                result /= paths[i];
            }
            return joinPath(result, paths.back());
        }

        inline std::string currentPath() 
        {
            return std::filesystem::current_path().string();
        }

        inline std::string sourcePath(bool parent_path = true) 
        {
            std::filesystem::path source_path;
            #if defined(_WIN32)
                char path[MAX_PATH];
                GetModuleFileName(NULL, path, MAX_PATH);
                source_path = path;
            #elif defined(__linux__) || defined(__apple__)
                source_path = filesystem::canonical("/proc/self/exe");
            #else
                throw std::runtime_error(_private::errorMessage(__func__, "Unknown Operating System"));
            #endif

            if(parent_path) {
                return source_path.parent_path().string();
            }

            return source_path.string();
        }

        inline std::string rootName(const std::filesystem::path& path)
        {
            return path.root_name().string();
        }

        inline bool createDirectory(const std::filesystem::path& path)
        {
            return std::filesystem::create_directories(path);
        }

        inline bool createFile(const std::filesystem::path& path, const std::string& data, const CopyOption& op = CopyOption::None)
        {
            if(op == CopyOption::SkipExisting) {
                return false;
            }

            if(!std::filesystem::exists(path) || op == CopyOption::OverwriteExisting || op == CopyOption::OverwriteAll) {
                std::ofstream file(path);
                file << data;
                file.close();
                return true;
            } else {
                char ch = _private::copyWarning(path.filename());
                if(ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                    std::ofstream file(path);
                    file << data;
                    file.close();
                    return true;
                }
                return false;
            }
        }

        inline bool createFile(const std::filesystem::path& path, const std::vector<std::string>& data, const CopyOption& op = CopyOption::None)
        {
            if(op == CopyOption::SkipExisting) {
                return false;
            }

            if(!std::filesystem::exists(path) || op == CopyOption::OverwriteExisting || op == CopyOption::OverwriteAll) {
                std::ofstream file(path);
                for(int i = 0; i < data.size(); i++) {
                    file << data[i];
                    if(i < data.size()-1) {
                        file << std::endl;
                    }
                }
                file.close();
                return true;
            } else {
                char ch = _private::copyWarning(path.filename());
                if(ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                    std::ofstream file(path);
                    for(int i = 0; i < data.size(); i++) {
                        file << data[i] << std::endl;
                        if(i < data.size()-1) {
                            file << std::endl;
                        }
                    }
                    file.close();
                    return true;
                }
                return false;
            }
        }

        inline bool createFile(const std::filesystem::path& path, const CopyOption& op = CopyOption::None)
        {
            return createFile(path, "", op);
        }

        inline void rename(const std::filesystem::path& path, const std::string& new_name)
        {
            std::filesystem::rename(path, path.parent_path() / new_name);
        }

        inline bool copy(const std::filesystem::path& from, const std::filesystem::path& to, const TraversalOption& traversal_option,
                        const CopyOption& copy_option = CopyOption::None)
        {
            return _private::copy(from, to, copy_option, traversal_option);
        }

        inline bool copy(const std::filesystem::path& from, const std::filesystem::path& to, const CopyOption& copy_option,
                         const TraversalOption& traversal_option = TraversalOption::Recursive)
        {
            return _private::copy(from, to, copy_option, traversal_option);
        }

        inline bool copy(const std::filesystem::path& from, const std::filesystem::path& to)
        {
            return _private::copy(from, to, CopyOption::None, TraversalOption::Recursive);
        }

        inline bool copy(const std::filesystem::path& from, const std::set<std::string>& paths_to_copy_in_from, const std::filesystem::path& to, const CopyOption& op = CopyOption::None)
        {
            return _private::copy(from, paths_to_copy_in_from, to, op);
        }

        inline bool move(const std::filesystem::path& from, const std::filesystem::path& to, const TraversalOption& traversal_option,
                        const CopyOption& copy_option = CopyOption::None)
        {
            return _private::move(from, to, copy_option, traversal_option);
        }

        inline bool move(const std::filesystem::path& from, const std::filesystem::path& to, const CopyOption& copy_option,
                         const TraversalOption& traversal_option = TraversalOption::Recursive)
        {
            return _private::move(from, to, copy_option, traversal_option);
        }

        inline bool move(const std::filesystem::path& from, const std::filesystem::path& to)
        {
            return _private::move(from, to, CopyOption::None, TraversalOption::Recursive);
        }

        inline bool move(const std::filesystem::path& from, const std::set<std::string>& paths_to_copy_in_from, const std::filesystem::path& to, const CopyOption& op = CopyOption::None)
        {
            return _private::move(from, paths_to_copy_in_from, to, op);
        }

        inline bool remove(const std::filesystem::path& path)
        {
            if(!std::filesystem::exists(path)) {
                return false;
            }

            if(std::filesystem::is_directory(path)) {
                if(isDirectoryString(path)) {
                    for(const auto& entry : std::filesystem::directory_iterator(path)) {
                        std::filesystem::remove_all(entry.path());
                    }
                } else {
                    std::filesystem::remove_all(path);
                }
            } else {
                std::filesystem::remove(path);
            }

            return true;
        }

        inline bool hasSameContent(const std::filesystem::path& p1, const std::filesystem::path& p2)
        {
            if(!std::filesystem::exists(p1)) {
                throw std::runtime_error(_private::errorMessage(__func__, "\"" + p1.string() + "\" does not exist"));
            }

            if(!std::filesystem::exists(p2)) {
                throw std::runtime_error(_private::errorMessage(__func__, "\"" + p2.string() + "\" does not exist"));
            }

            bool is_p1_dir = std::filesystem::is_directory(p1);
            bool is_p2_dir = std::filesystem::is_directory(p2);

            if(is_p1_dir && !is_p2_dir || !is_p1_dir && is_p2_dir) {
                throw std::runtime_error(_private::errorMessage(__func__, "Arguments need to be both files or both folders"));
            }

            if(is_p1_dir && is_p2_dir) {
                auto i = std::filesystem::recursive_directory_iterator(p1);
                auto j = std::filesystem::recursive_directory_iterator(p2);
                while(i != std::filesystem::recursive_directory_iterator() && j != std::filesystem::recursive_directory_iterator()) {
                    if(std::filesystem::relative(i->path(), p1) != std::filesystem::relative(j->path(), p2)) {
                        return false;
                    }
                    i++;
                    j++;
                }

                if(i == std::filesystem::recursive_directory_iterator() && j == std::filesystem::recursive_directory_iterator()) {
                    return true;
                }

                return false;
            } else {
                std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
                std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

                if(f1.fail() || f2.fail()) {
                    return false;
                }

                if(f1.tellg() != f2.tellg()) {
                    return false;
                }

                f1.seekg(0, std::ifstream::beg);
                f2.seekg(0, std::ifstream::beg);
                return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                                    std::istreambuf_iterator<char>(),
                                    std::istreambuf_iterator<char>(f2.rdbuf()));
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
                throw std::runtime_error(_private::errorMessage(__func__, "Path does not exist"));
            }
        }

        inline std::string find(const std::filesystem::path& search_path, const std::string& file_to_find, const TraversalOption& pt = TraversalOption::NonRecursive)
        {
            int n = pt == TraversalOption::NonRecursive ? 0 : -1;
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
                throw std::runtime_error(_private::errorMessage(__func__, "Path does not exist"));
            }
        }

        inline std::vector<std::string> findAll(const std::filesystem::path& search_path, const std::string& file_to_find, const TraversalOption& pt = TraversalOption::NonRecursive)
        {
            int n = pt == TraversalOption::NonRecursive ? 0 : -1;
            return path::findAll(search_path, file_to_find, n);
        }

        namespace _private {

            inline std::string errorMessage(const std::string& function_name, const std::string& message)
            {
                std::string error = "[Error]";
                #ifdef _DEBUG
                    error.append("[" + function_name + "]");
                #endif
                error.push_back(' ');
                error.append(message);

                return error;
            }

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
                std::filesystem::path parent_temp = to.parent_path();
                if(!parent_temp.empty() && !std::filesystem::exists(parent_temp)) {
                    std::filesystem::create_directories(parent_temp);
                }

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

            inline bool copy(const std::filesystem::path& source, const std::filesystem::path& destination, 
                             const CopyOption& op, const TraversalOption& t_op)
            {
                if(!std::filesystem::exists(source)) {
                    throw std::runtime_error(_private::errorMessage(__func__, "\"" + source.string() + "\" does not exist"));
                }

                char ch;
                std::filesystem::path from = source;
                std::filesystem::path to = destination;
                if(std::filesystem::is_directory(from)) { // is directory

                    // Create directory when destination does not exists
                    if(!std::filesystem::exists(to)) {
                        std::filesystem::create_directories(to);
                    }

                    // Throw an error if a directory is being copied into a file
                    if(!std::filesystem::is_directory(to)) {
                        throw std::runtime_error(_private::errorMessage(__func__, "\"" + to.filename().string() + "\" is a file"));
                    }

                    // Remove all contents of directory when "OverwriteAll" option is active
                    if(op == CopyOption::OverwriteAll) {
                        for(const auto& entry : std::filesystem::directory_iterator(to)) {
                            path::remove(entry.path());
                        }
                    } 

                    // store the paths first before copying to prevent endless recursion
                    std::vector<std::filesystem::path> paths;
                    if(t_op == TraversalOption::Recursive) {
                        // Get relative path to conserve memory
                        for(const auto& entry : std::filesystem::recursive_directory_iterator(from)) {
                            paths.push_back(std::filesystem::relative(entry.path(), from));
                        }
                    }

                    // If "from" has a trailing separator, copy "from" directory with subdirectories
                    if(!isDirectoryString(from)) {
                        to = std::filesystem::weakly_canonical(to / from.filename());
                        std::filesystem::create_directories(to);
                        
                        if(t_op == TraversalOption::NonRecursive) {
                            return true;
                        }
                    } else if(t_op == TraversalOption::NonRecursive) {
                        for(const auto& entry : std::filesystem::directory_iterator(from)) {
                            paths.push_back(std::filesystem::relative(entry.path(), from));
                        }
                    }

                    for(int i = 0; i < paths.size(); i++) {
                        std::filesystem::path source = std::filesystem::weakly_canonical(from / paths[i]);
                        std::filesystem::path copy_to = std::filesystem::weakly_canonical(to / paths[i]);
                        bool is_source_dir = std::filesystem::is_directory(source);
                        bool destination_exists = std::filesystem::exists(copy_to);
                        
                        // display warning
                        if(op == CopyOption::None && destination_exists && ch != 'a' && ch != 'A') {
                            ch = _private::copyWarning(path::relativePath(copy_to));
                        }

                        if(ch == 'x' || ch == 'X') {
                            return false;
                        }

                        if(is_source_dir) { 
                            std::filesystem::create_directories(copy_to);
                        } else if(!destination_exists || op == CopyOption::OverwriteExisting || ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                            _private::copyFile(source, copy_to);
                        } 
                    }
                } else { // is file
                    if(isDirectoryString(from)) {
                        from = from.parent_path();
                    }

                    bool is_source_dir = std::filesystem::is_directory(from);
                    bool is_destination_dir = std::filesystem::is_directory(to);

                    if(is_destination_dir && op == CopyOption::OverwriteAll) {
                        for(const auto& entry : std::filesystem::directory_iterator(to)) {
                            path::remove(entry.path());
                        }
                    } 

                    std::filesystem::path copy_to = std::filesystem::is_directory(to) ? std::filesystem::weakly_canonical(to / path::filename(from)) : to;
                    bool destination_exists = std::filesystem::exists(copy_to);

                    if(op == CopyOption::None && destination_exists && ch != 'a' && ch != 'A') {
                        ch = _private::copyWarning(path::relativePath(copy_to));
                    }

                    if(ch == 'x' || ch == 'X') {
                        return false;
                    }

                    if(is_source_dir) { 
                        std::filesystem::create_directories(copy_to);
                    } else if(!destination_exists || op == CopyOption::OverwriteExisting || ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                        _private::copyFile(from, copy_to);
                    } 
                }

                return true;
            }

            inline bool copy(const std::filesystem::path& source, const std::set<std::string>& paths, 
                             const std::filesystem::path& destination, const CopyOption& op)
            {
                if(!std::filesystem::exists(source)) {
                    throw std::runtime_error(_private::errorMessage(__func__, "\"" + source.string() + "\" does not exist"));
                }

                if(op == CopyOption::OverwriteAll) {
                    for(const auto& entry : std::filesystem::directory_iterator(destination)) {
                        path::remove(entry.path());
                    }
                }

                char ch;
                for(const auto& i : paths) {
                    std::filesystem::path from = std::filesystem::weakly_canonical(source / i);
                    std::filesystem::path to = std::filesystem::weakly_canonical(destination / std::filesystem::relative(from, source));

                    bool destination_exists = std::filesystem::exists(to);
                    bool is_source_dir = std::filesystem::is_directory(from);
                    
                    // display warning
                    if(op == CopyOption::None && destination_exists && ch != 'a' && ch != 'A') {
                        ch = _private::copyWarning(path::relativePath(to));
                    }

                    if(ch == 'x' || ch == 'X') {
                        return false;
                    }

                    if(is_source_dir) { 
                        std::filesystem::create_directories(to);
                    } else if(!destination_exists || op == CopyOption::OverwriteExisting || ch == 'y' || ch == 'Y' || ch == 'a' || ch == 'A') {
                        _private::copyFile(from, to);
                    } 
                }

                return true;
            }

            inline bool move(const std::filesystem::path& source, const std::filesystem::path& destination, 
                             const CopyOption& op, const TraversalOption& t_op)
            {
                if(!_private::copy(source, destination, op, t_op)) {
                    return false;
                }

                path::remove(source);
                return true;
            }

            inline bool move(const std::filesystem::path& source, const std::set<std::string>& paths, 
                             const std::filesystem::path& destination, const CopyOption& op)
            {
                if(!_private::copy(source, paths, destination, op)) {
                    return false;
                }

                for(auto it = paths.rbegin(); it != paths.rend(); it++) {
                    std::string full_path = path::joinPath(source, *it);

                    if(isDirectory(full_path) && !isEmpty(full_path)) {
                        continue;
                    }

                    path::remove(full_path);
                }

                return true;
            }
        }
    }

    inline std::string execute(const std::string& command, const std::string& mode = "r")
    {
        FILE* pipe = popen(command.c_str(), mode.c_str());
        if(!pipe) {
            throw std::runtime_error("Execution Failed");
        }

        char buffer[128];
        std::string output;
        while(fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        pclose(pipe);
        return output;
    }
}