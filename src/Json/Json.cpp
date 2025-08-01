#include "Json.h"

#include "../App/App.h"
#include "../Runner/Runner.h"

// Serialize ImVec4 to JSON
void to_json(json& j, const ImVec4& v) {
    j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
}

// Serialize std::vector<std::pair<std::wstring, std::wstring>> to JSON
void to_json(nlohmann::json& j, const std::vector<std::pair<std::wstring, std::wstring>>& vec) {
    j = nlohmann::json::object();
    for (const auto& [key, value] : vec) {
        std::string skey(key.begin(), key.end());
        std::string svalue(value.begin(), value.end());
        j[skey] = svalue;
    }
}

// Deserialize JSON to ImVec4
void from_json(const json& j, ImVec4& v) {
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
    j.at("w").get_to(v.w);
}

//Deserialize JSON to std::vector<std::pair<std::wstring, std::wstring>>
void from_json(const nlohmann::json& j, std::vector<std::pair<std::wstring, std::wstring>>& vec) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::wstring wkey(it.key().begin(), it.key().end());
        auto wval = it.value().get<std::wstring>();
        vec.emplace_back(std::move(wkey), std::move(wval));
    }
}

// Combine loader
AppConfig LoadConfig() {
    AppConfig config;

    if (fs::exists(FILENAME)) {
        std::ifstream inFile(FILENAME);
        if (inFile) {
            json j;
            inFile >> j;

            // Load font
            if (j.contains("font_path")) {
                config.fontPath = j["font_path"].get<std::string>();
                if (!fs::exists(config.fontPath)) {
                    config.fontPath = FONT_PATH_DEF;
                }
            } else {
                config.fontPath = FONT_PATH_DEF;
            }

            // Load colors
            if (j.contains("colors")) {
                for (auto& [key, value] : j["colors"].items()) {
                    config.colors[key] = value.get<ImVec4>();
                }
            }

            if (j.contains("vk_bind")) {
                config.keybindVK = j["vk_bind"].get<uint32_t>();
            } else {
                config.keybindVK = VK_SPACE;
            }

            if (j.contains("mod_bind")) {
                config.keybindMod = j["mod_bind"].get<uint32_t>();
            } else {
                config.keybindMod = MOD_CONTROL;
            }

            if (j.contains("custom_commands")) {
                config.customCommands = j["custom_commands"].get<std::vector<std::pair<std::wstring, std::wstring>>>();
                Runner::knownCommands = Runner::knownCommandsOriginal;
                for (auto& cmd : config.customCommands) {
                    Runner::knownCommands.emplace(cmd.first);
                    Runner::customCommands.emplace(cmd);
                }
            } else {
                config.customCommands = std::vector<std::pair<std::wstring, std::wstring>>();
            }
        }
    } else {
        // Use defaults
        config.fontPath = FONT_PATH_DEF;
        config.colors["background"] = WINDOW_COLOR_DEF;
        config.colors["frame"] = FRAME_COLOR_DEF;
        config.colors["text"] = TEXT_COLOR_DEF;
        config.keybindMod = MOD_CONTROL;
        config.keybindVK = VK_SPACE;
        config.customCommands = std::vector<std::pair<std::wstring, std::wstring>>();

        // Save default config
        json j;
        j["font_path"] = config.fontPath;
        j["colors"] = config.colors;
        j["vk_bind"] = config.keybindVK;
        j["mod_bind"] = config.keybindMod;

        std::ofstream outFile(FILENAME);
        outFile << j.dump(4);
    }


    return config;
}

void SaveConfig(const AppConfig& config) {
    json j;

    // Save font path
    j["font_path"] = config.fontPath;

    // Save color map
    j["colors"] = json::object();
    for (const auto& [key, color] : config.colors) {
        j["colors"][key] = color;
    }

    j["vk_bind"] = config.keybindVK;
    j["mod_bind"] = config.keybindMod;

    j["custom_commands"] = config.customCommands;

    // Write to file
    std::ofstream outFile(FILENAME);
    if (outFile) {
        outFile << j.dump(4);
    }
}