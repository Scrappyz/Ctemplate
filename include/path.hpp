#include <string>
#include <filesystem>
#include <type_traits>
#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
#endif

template<typename T>
bool pathExists(const T& path) // checks if the path exists
{
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::filesystem::path>,
        "Argument must be std::string or std::filesystem::path");

    return std::filesystem::exists(path);
}

bool pathExists(const char* path) // for string literals
{
    return pathExists(std::string(path));
}

bool validFilenameChar(char ch) // checks if character passed is a valid character for filenames
{
    switch(ch) {
        case '#':
        case '%':
        case '&':
        case '{':
        case '}':
        case '\\':
        case '/':
        case '<':
        case '>':
        case '*':
        case '?':
        case '~':
        case '!':
        case '\'':
        case '\"':
        case ':':
        case '@':
        case '`':
        case '|':
        case '=':
            return false;
        default:
            return true;
    }
}

bool isDirectorySeparator(char ch) // checks if the character passed is a directory separator
{
    char preferred = std::filesystem::path::preferred_separator;
    return ch == preferred || ch == '/' && preferred == '\\';
}

template<typename T>
bool isFile(const T& path) // checks if path is a file
{
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::filesystem::path>,
        "Argument must be std::string or std::filesystem::path");
    
    return std::filesystem::is_regular_file(path);
}

bool isFile(const char* path) // for string literals
{
    return isFile(std::string(path));
}

template<typename T>
bool isDirectory(const T& path) // checks if path is a directory
{
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::filesystem::path>,
        "Argument must be std::string or std::filesystem::path");

    return std::filesystem::is_directory(path);
}

bool isDirectory(const char* path) // for string literals
{
    return isDirectory(std::string(path));
}

char directorySeparator() // returns the preferred directory separator of your operating system
{
    return std::filesystem::path::preferred_separator;
}

std::string getCurrentPath() // returns the current path you are in the command line
{
    char currentPath[MAX_PATH];
    #if defined(_WIN32)
        GetCurrentDirectory(MAX_PATH, currentPath);
    #elif defined(__linux__)
        getcwd(currentPath, MAX_PATH);
    #endif
    return std::string(currentPath);
}

std::string getSourcePath() // returns the path to the executable
{
    char path[MAX_PATH];
    std::filesystem::path pth;
    #if defined(_WIN32)
        GetModuleFileName(NULL, path, MAX_PATH);
        pth = path;
    #elif defined(__linux__)
        pth = filesystem::canonical("/proc/self/exe");
    #else
        cerr << "[Error] Unknown Operating System" << endl;
        return string();
    #endif
    return pth.parent_path().string();
}

std::string getFilename(const std::string& path)
{
    return std::filesystem::path(path).filename().string();
}

template<typename T1, typename T2>
std::string relativePath(const T1& existing_path, const T2& base_path) // subtracts the existing_path from the base_path
{
    static_assert(std::is_same_v<T1, std::string> || std::is_same_v<T1, std::filesystem::path>,
        "T1 must be either std::string or std::filesystem::path");

    static_assert(std::is_same_v<T2, std::string> || std::is_same_v<T2, std::filesystem::path>,
        "T2 must be either std::string or std::filesystem::path");

    return std::filesystem::relative(existing_path, base_path).string();
}

template<typename T1>
std::string relativePath(const T1& existing_path, const char* base_path)
{
    static_assert(std::is_same_v<T1, std::string> || std::is_same_v<T1, std::filesystem::path>,
        "T1 must be either std::string or std::filesystem::path");

    return relativePath(existing_path, std::string(base_path));
}

template<typename T2>
std::string relativePath(const char* existing_path, const T2& base_path)
{
    static_assert(std::is_same_v<T2, std::string> || std::is_same_v<T2, std::filesystem::path>,
        "T2 must be either std::string or std::filesystem::path");

    return relativePath(std::string(existing_path), base_path);
}

std::string relativePath(const char* existing_path, const char* base_path)
{
    return relativePath(std::string(existing_path), std::string(base_path));
}

template<typename T1, typename T2>
std::string joinPath(const T1& path, const T2& child_path) // concatenates two paths together
{
    static_assert(std::is_same_v<T1, std::string> || std::is_same_v<T1, std::filesystem::path>,
        "T1 must be either std::string or std::filesystem::path");

    static_assert(std::is_same_v<T2, std::string> || std::is_same_v<T2, std::filesystem::path>,
        "T2 must be either std::string or std::filesystem::path");

    std::filesystem::path p1(path);
    std::filesystem::path p2(child_path);
    return std::filesystem::weakly_canonical(p1 / p2).string();
}

template<typename T1>
std::string joinPath(const T1& path, const char* child_path)
{
    static_assert(std::is_same_v<T1, std::string> || std::is_same_v<T1, std::filesystem::path>,
        "T1 must be either std::string or std::filesystem::path");

    return joinPath(path, std::string(child_path));
}

template<typename T2>
std::string joinPath(const char* path, const T2& child_path)
{
    static_assert(std::is_same_v<T2, std::string> || std::is_same_v<T2, std::filesystem::path>,
        "T2 must be either std::string or std::filesystem::path");
    
    return joinPath(std::string(path), child_path);
}

std::string joinPath(const char* path, const char* child_path) 
{
    return joinPath(std::string(path), std::string(child_path));
}