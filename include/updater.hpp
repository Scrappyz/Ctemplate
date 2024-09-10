#pragma once

#include "json.hpp"
#include <string>
#include <cstdio>
#include <filesystem>

#if defined(_WIN32)
    #include <windows.h>
    #include <strsafe.h>
#else
    #include <unistd.h>
    #include <libgen.h>
#endif

namespace updater {

    #if defined(_WIN32)
        // Path to cURL. (Defaults "curl.exe")
        inline std::string curl_path = "curl.exe";
    #else
        // Path to cURL. (Defaults "curl")
        inline std::string curl_path = "curl";
    #endif

    // URL to Github.
    inline const std::string github_url = "https://github.com/";

    // URL to Github API.
    inline const std::string github_api_url = "https://api.github.com/repos/";

    namespace _private_ {
        bool execute(const std::string& command, std::string& output, const std::string& mode = "r");
        bool execute(const std::string& command, const std::string& mode = "r");
        nlohmann::json getReleaseJson(const std::string& api_url, const std::string& tag);
        nlohmann::json getLatestReleaseJson(const std::string& api_url, bool pre_release = false);
        nlohmann::json getTagListJson(const std::string& api_url);
        bool downloadAsset(const std::string& download_url, const std::filesystem::path& output_path);
        bool updateApp(const nlohmann::json& release_info, const std::string& asset_name);
    }

    inline std::string sourcePath(bool parent_path = true) 
    {
        std::filesystem::path source_path;
        #if defined(_WIN32)
            char path[MAX_PATH];
            GetModuleFileName(NULL, path, MAX_PATH);
            source_path = path;
        #elif defined(__linux__) || defined(__apple__)
            source_path = std::filesystem::canonical("/proc/self/exe");
        #else
            throw std::runtime_error(_private::errorMessage(__func__, "Unknown Operating System"));
        #endif

        if(parent_path) {
            return source_path.parent_path().string();
        }

        return source_path.string();
    }

    #if defined(_WIN32)
        /*
            Deletes the running executable.

            Parameters:
            `source_path`: Path to the running executable. (Defaults to running executable)

            Notes:
            - Only works if the program is terminated after 2 seconds so it would be best to run this right before program exit.
        */
        inline void removeSelf(const std::filesystem::path& source_path = "")
        {
            std::string remove_cmd = "cmd.exe /C ping 1.1.1.1 -n 1 -w 2000 > Nul & Del /f /q \"%s\"";
            TCHAR szCmd[2 * MAX_PATH];
            STARTUPINFO si = {0};
            PROCESS_INFORMATION pi = {0};

            std::string source_path_str = source_path.string();
            if(source_path_str.empty()) {
                source_path_str = sourcePath(false);
            }

            StringCbPrintf(szCmd, 2 * MAX_PATH, remove_cmd.c_str(), source_path_str.c_str());

            CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    #else
        /*
            Deletes the running executable.

            Parameters:
            `source_path`: Path to the running executable. (Defaults to running executable)

            Notes:
            - Only works if the program is terminated after 2 seconds so it would be best to run this right before program exit.
        */
        inline void removeSelf(const std::filesystem::path& source_path = "")
        {
            std::string path_to_remove = source_path.empty() ? sourcePath(false) : source_path.string();
            char command[2 * 1024];

            snprintf(command, sizeof(command), "sleep 3 && rm -f \"%s\"", path_to_remove.c_str());

            if(fork() == 0) {
                execl("/bin/sh", "sh", "-c", command, (char *)NULL);
                exit(EXIT_SUCCESS);
            }
        }
    #endif

    /*
        Returns the github url of the given username and repo.

        Parameters:
        `user_name`: Username of the owner of the repo.
        `repo_name`: Name of the repo.
    */
    inline std::string getUrl(const std::string& user_name, const std::string& repo_name)
    {
        std::string result = github_url + user_name + "/" + repo_name;
        return result;
    }

    /*
        Converts the given github repo url to an API url.

        Parameters:
        `repo_url`: URL to the github repository. (E.g. `https://github.com/{USER}/{REPO}`)
    */
    inline std::string getAPIUrl(const std::string& repo_url)
    {
        std::string result = github_api_url + repo_url.substr(19, repo_url.size() - 19);
        return result;
    }

    /*
        Returns the API url of the given username and repo.

        Parameters:
        `user_name`: Username of the owner of the repo.
        `repo_name`: Name of the repo.
    */
    inline std::string getAPIUrl(const std::string& user_name, const std::string& repo_name)
    {
        std::string result = github_api_url + user_name + "/" + repo_name;
        return result;
    }

    /*
        Gets the release information from github of the given tag.

        Parameters:
        `repo_url`: URL to the github repository. (E.g. `https://github.com/{USER}/{REPO}`)
        `tag`: The tag name to get.
    */
    inline nlohmann::json getReleaseJson(const std::string& repo_url, const std::string& tag)
    {
        std::string api_url = getAPIUrl(repo_url);
        return _private_::getReleaseJson(api_url, tag);
    }

    /*
        Gets the latest release information from github.

        Parameters:
        `repo_url`: URL to the github repository. (E.g. `https://github.com/{USER}/{REPO}`)
        `pre_release`: Allow pre-releases.
    */
    inline nlohmann::json getLatestReleaseJson(const std::string& repo_url, bool pre_release = false)
    {
        std::string api_url = getAPIUrl(repo_url);
        return _private_::getLatestReleaseJson(api_url, pre_release);
    }

    /*
        Gets the list of tags.

        Parameters:
        `repo_url`: URL to the github repository. (E.g. `https://github.com/{USER}/{REPO}`)
    */
    inline nlohmann::json getTagListJson(const std::string& repo_url)
    {
        std::string api_url = getAPIUrl(repo_url);
        return _private_::getTagListJson(api_url);
    }

    /*
        Downloads an asset from a given repository and tag.

        Parameters:
        `repo_url`: URL to the repo. (E.g. `https://github.com/{USER}/{REPO}`)
        `tag`: Tag to download asset from.
        `asset_name`: Name of the asset to download from the given tag.
        `output_path`: Path to the output file.
    */
    inline bool downloadAsset(const std::string& repo_url, const std::string& tag, const std::string& asset_name, const std::filesystem::path& output_path)
    {
        std::string download_url = repo_url + "/releases/download/" + tag + "/" + asset_name;
        return _private_::downloadAsset(download_url, output_path);
    }

    /*
        Updates the application by replacing the current app with the new one.

        Parameters:
        `repo_url`: URL to the github repo.
        `tag`: Tag to update application to.
        `asset_name`: The asset to download.

        Notes:
        - Only works if the program is terminated after 2 seconds so it would be best to run this right before program exit.
    */
    inline bool updateApp(const std::string& repo_url, const std::string& tag, const std::string& asset_name)
    {
        nlohmann::json release_info;

        if(tag.empty()) {
            release_info = getLatestReleaseJson(repo_url);
        } else {
            release_info = getReleaseJson(repo_url, tag);
        }
        
        return _private_::updateApp(release_info, asset_name);
    }

    /*
        Updates the application by replacing the current app with the new one.

        Parameters:
        `repo_url`: URL to the github repo.
        `asset_name`: The asset to download.

        Notes:
        - Only works if the program is terminated after 2 seconds so it would be best to run this right before program exit.
    */
    inline bool updateApp(const std::string& repo_url, const std::string& asset_name)
    {
        return updateApp(repo_url, "", asset_name);
    }

    /*
        Checks if cURL is installed.
    */
    inline bool isCurlInstalled()
    {
        return _private_::execute(curl_path + " --help");
    }

    /*
        Checks if the JSON request for the Github API was found.
    */
    inline bool isJsonFound(const nlohmann::json& j)
    {
        return !(j.contains("message") && j.at("message") == "Not Found");
    }

    namespace _private_ {

        /*
            Execute a command in the shell.

            Return Value:
            - Returns `true` if the command executes successfully. Else it will return `false`.

            Parameters:
            `command`: Command to execute.
            `output`: A string reference to get the output of the executed command.
            `mode`: Use `r` for read-mode, `w` for write-mode.
        */
        inline bool execute(const std::string& command, std::string& output, const std::string& mode)
        {
            // Open the pipe using platform-specific popen function
            FILE* pipe = popen(command.c_str(), mode.c_str());
            if(!pipe) {
                return false;  // Command failed to execute
            }

            char buffer[128];
            output.clear();  // Clear the output string before capturing the result

            // Read the command's output chunk by chunk
            while(fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                output.append(buffer);  // Append each chunk to the output string
            }

            // Get the command exit status
            int exit_code = pclose(pipe);
            
            // Return true if the command was successful (exit code is 0)
            return (exit_code == 0);
        }

        /*
            Execute a command in the shell.

            Return Value:
            - Returns `true` if the command executes successfully. Else it will return `false`.

            Parameters:
            `command`: Command to execute.
            `mode`: Use `r` for read-mode, `w` for write-mode.
        */
        inline bool execute(const std::string& command, const std::string& mode)
        {
            std::string dummy_output;
            return execute(command, dummy_output, mode);
        }

        inline nlohmann::json getReleaseJson(const std::string& api_url, const std::string& tag)
        {
            std::string output;

            _private_::execute(curl_path + " -s " + api_url + "/releases/tags/" + tag, output);

            nlohmann::json result = nlohmann::json::parse(output);

            if(isJsonFound(result)) {
                return result;
            }

            return nlohmann::json();
        }

        inline nlohmann::json getLatestReleaseJson(const std::string& api_url, bool pre_release)
        {
            std::string output;
            nlohmann::json result;

            if(!pre_release) {
                _private_::execute(curl_path + " -s " + api_url + "/releases/latest", output);
                result = nlohmann::json::parse(output);
            } else {
                _private_::execute(curl_path + " -s " + api_url + "/releases", output);
                result = nlohmann::json::parse(output)[0];
            }

            
            if(isJsonFound(result)) {
                return result;
            }

            return nlohmann::json();
        }

        inline nlohmann::json getTagListJson(const std::string& api_url)
        {
            std::string output;

            _private_::execute(curl_path + " -s " + api_url + "/tags", output);

            nlohmann::json result = nlohmann::json::parse(output);

            if(isJsonFound(result)) {
                return result;
            }

            return nlohmann::json();
        }

        inline bool downloadAsset(const std::string& download_url, const std::filesystem::path& output_path)
        {
            std::string command = curl_path + " -s -L " + download_url + " -o " + "\"" + output_path.string() + "\"";
            return _private_::execute(command);
        }

        inline bool updateApp(const nlohmann::json& release_info, const std::string& asset_name)
        {
            std::filesystem::path source_path = sourcePath(false);
            std::filesystem::path source_temp = source_path.string() + "1";
            std::filesystem::path new_source_path = source_path.string() + "2";

            for(const auto& i : release_info.at("assets")) {
                if(i.at("name") == asset_name) {
                    _private_::downloadAsset(i.at("browser_download_url"), source_temp);
                    break;
                }
            }

            if(!std::filesystem::exists(source_temp)) {
                return false;
            }

            std::filesystem::rename(source_path, new_source_path);
            std::filesystem::rename(source_temp, source_path);

            removeSelf(new_source_path);

            return true;
        }
    }
}
