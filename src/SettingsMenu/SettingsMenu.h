#pragma once

#include <string>
#include <windows.h>
#include "imgui.h"
#include "../Json/json.h"
#include "../helpers/helper.h"

#define BUFFER_MIN 1024

class SettingsMenu {
private:
    int width, height;
    ImVec4 frameColor, textColor, backgroundColor;
    char fontPath[256];
    int keybindVK = 0;
    int keybindMod = 0;
    ImGuiKey keybindKey = ImGuiKey_None;
    std::string bindWaiting = "";
    std::vector<std::pair<std::wstring, std::wstring>> customs;
    std::vector<std::pair<char[BUFFER_MIN], char[BUFFER_MIN]>> editBuffers;
    void SaveSettings();

public:
    bool shouldClose = false;
    bool shouldUpdate = false;
    bool shouldUpdateBinds = false;
    bool shouldUpdateCommands = false;
    SettingsMenu(int width, int height);
    void Draw();
};
