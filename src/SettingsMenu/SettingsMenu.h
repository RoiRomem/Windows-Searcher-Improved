#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include "imgui.h"
#include "../Json/json.h"
#include "../helpers/helper.h"

#define BUFFER_MIN 1024

struct CommandBuffer {
    char prompt[BUFFER_MIN];
    char command[BUFFER_MIN];
};

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
    std::vector<CommandBuffer> editBuffers;
    
    void SaveSettings();

public:
    bool shouldClose = false;
    bool shouldUpdate = false;
    bool shouldUpdateBinds = false;
    bool shouldUpdateCommands = false;
    
    SettingsMenu(int width, int height);
    void Draw();
};