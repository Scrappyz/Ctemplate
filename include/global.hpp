#pragma once

#include <string>
#include "json.hpp"

namespace global {

    extern std::string github_url;
    extern std::string app_version;
    extern std::string asset_name;
    extern nlohmann::json app_config;
    extern nlohmann::json template_info_config;
    extern nlohmann::json template_variables_config;
    extern std::string cache_container_name;
    
}