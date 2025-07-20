#include "Json.h"

// Serialize ImVec4 to JSON
void to_json(json& j, const ImVec4& v) {
    j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
}

// Deserialize JSON to ImVec4
void from_json(const json& j, ImVec4& v) {
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
    j.at("w").get_to(v.w);
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
        }
    } else {
        // Use defaults
        config.fontPath = FONT_PATH_DEF;
        config.colors["window"] = WINDOW_COLOR_DEF;
        config.colors["frame"] = FRAME_COLOR_DEF;
        config.colors["text"] = TEXT_COLOR_DEF;

        // Save default config
        json j;
        j["font_path"] = config.fontPath;
        j["colors"] = config.colors;

        std::ofstream outFile(FILENAME);
        outFile << j.dump(4);
    }

    return config;
}