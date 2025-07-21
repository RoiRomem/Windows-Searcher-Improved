#pragma once

#include <string>
#include "imgui.h"
#include "../Json/json.h"

class SettingsMenu {
private:
    int width, height;
    ImVec4 frameColor, textColor, backgroundColor;
    char fontPath[256];
    void SaveSettings();

public:
    bool shouldClose = false;
    bool shouldUpdate = false;
    SettingsMenu(int width, int height);
    void Draw();
};
