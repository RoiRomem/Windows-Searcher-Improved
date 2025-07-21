#include "SettingsMenu.h"
#include <cstring>

SettingsMenu::SettingsMenu(int width, int height)
    : width(width), height(height) {

    auto cnf = LoadConfig();
    frameColor = cnf.colors["frame"];
    backgroundColor = cnf.colors["background"];
    textColor = cnf.colors["text"];

    // Copy font path safely into buffer
    strncpy(fontPath, cnf.fontPath.c_str(), sizeof(fontPath));
    fontPath[sizeof(fontPath) - 1] = '\0';
}

void SettingsMenu::Draw() {
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::ColorEdit4("Frame Color", reinterpret_cast<float*>(&frameColor));
    ImGui::ColorEdit4("Text Color", reinterpret_cast<float*>(&textColor));
    ImGui::ColorEdit4("Background Color", reinterpret_cast<float*>(&backgroundColor));
    ImGui::InputText("Font (path)", fontPath, sizeof(fontPath));

    ImGui::Separator();
    if (ImGui::Button("Save")) {
        SaveSettings();
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        shouldClose = true;
    }

    ImGui::End();
}

void SettingsMenu::SaveSettings() {
    auto nConfig = AppConfig();
    nConfig.colors["frame"] = frameColor;
    nConfig.colors["background"] = backgroundColor;
    nConfig.colors["text"] = textColor;
    nConfig.fontPath = fontPath;

    SaveConfig(nConfig);
    shouldUpdate = true;
}
