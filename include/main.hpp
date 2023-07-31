#pragma once

#include "cli.hpp"
#include "config.hpp"

void printHelp(const CLI& cli, const Config& config);
void printVersion(const std::string& program);
void setup();
void listTemplates(const Config& config);
void initTemplate(const CLI& cli, const Config& config);
void addTemplate(const CLI& cli, const Config& config);
void removeTemplate(const CLI& cli, const Config& config);
void editTemplate(const CLI& cli, const Config& config);
void setAll(CLI& cli);