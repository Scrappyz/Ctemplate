#include <string>
#include <filesystem>
#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
#endif

bool pathExists(const std::string& path)
{
    return std::filesystem::exists(std::filesystem::path(path));
}

bool validFilenameChar(char ch)
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

bool isFile(const std::string& path)
{
    std::filesystem::path p = path;
    return std::filesystem::is_regular_file(p);
}

bool isDirectory(const std::string& path)
{
    std::filesystem::path p = path;
    return std::filesystem::is_directory(p);
}

bool isDirectorySeparator(char ch)
{
    char preferred = std::filesystem::path::preferred_separator;
    return ch == preferred || ch == '/' && preferred == '\\';
}

char directorySeparator()
{
    return std::filesystem::path::preferred_separator;
}

std::string getCurrentPath()
{
    char currentPath[MAX_PATH];
    #if defined(_WIN32)
        GetCurrentDirectory(MAX_PATH, currentPath);
    #elif defined(__linux__)
        getcwd(currentPath, MAX_PATH);
    #endif
    return std::string(currentPath);
}

std::string getSourcePath()
{
    char path[MAX_PATH];
    std::filesystem::path pth;
    #if defined(_WIN32)
        GetModuleFileName(NULL, path, MAX_PATH);
        pth = path;
    #elif defined(__linux__)
        pth = filesystem::canonical("/proc/self/exe");
    #else
        return getCurrentPath();
    #endif
    return pth.parent_path().string();
}

std::string joinPath(const std::string& path, const std::string& child_path)
{
    std::filesystem::path p1(path);
    std::filesystem::path p2(child_path);
    return std::filesystem::weakly_canonical(p1 / p2).string();
}