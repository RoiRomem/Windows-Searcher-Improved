#pragma once

#include "json.hpp"
#include "imgui.h"
#include <fstream>
#include <filesystem>

#define FILENAME "settings.json"
#define WINDOW_COLOR_DEF ImVec4(0.1f, 0.1f, 0.1f, 1)
#define FRAME_COLOR_DEF ImVec4(0.2f, 0.2f, 0.2f, 0.6f)
#define TEXT_COLOR_DEF ImVec4(1, 1, 1, 1)
#define FONT_PATH_DEF "c:\\Windows\\Fonts\\segoeui.ttf"

using json = nlohmann::json;
namespace fs = std::filesystem;

struct AppConfig {
    std::string fontPath;
    std::unordered_map<std::string, ImVec4> colors;
};

AppConfig LoadConfig();
void SaveConfig(const AppConfig& config);
void from_json(const json& j, ImVec4& v);
void to_json(json& j, const ImVec4& v);