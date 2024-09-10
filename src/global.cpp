#include "global.hpp"
#include "os.hpp"

using json = nlohmann::json;
namespace path = os::path;

namespace global {

    std::string github_url = "https://github.com/Scrappyz/Ctemplate";
    std::string app_version = "v1.0.0-beta.3";

    #ifdef _WIN32
        std::string asset_name = "ctemplate.exe";
    #else
        std::string asset_name = "ctemplate";
    #endif

    json app_config = {
        {"templateDirectory", path::joinPath(path::sourcePath(), "templates")},
        {"containerName", ".ctemplate"}
    };

    json template_info_config = {
        {"author", ""},
        {"description", ""}
    };

    json template_variables_config = json::parse(R"(
        {
            "searchPaths": {
                "files": {
                    "include": [],
                    "exclude": []
                },
                "filenames": {
                    "include": [],
                    "exclude": []
                }
            },
            "variablePrefix": "!",
            "variableSuffix": "!",
            "variables": {}
        }
    )");
    
    std::string cache_container_name = ".cache";
}