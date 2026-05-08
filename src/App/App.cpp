#include "App.h"

#include <imgui_internal.h>

#include "../InputBuf/InputBuf.h"
#include "../SettingsMenu/SettingsMenu.h"
#include "../helpers/AppLogs.h"

App::App(const int width, const int height, const int setWidth, const int setHeight) {
    inputBuf = std::make_unique<InputBuf>(width, height, this);
    settingsMenu = std::make_unique<SettingsMenu>(setWidth, setHeight);
}

void App::Run() {
  if (inputBuf->getRequestedClose()) done = true;
    if (settingsMenu->shouldUpdateCommands) {
        inputBuf->UpdateCacheCommands();
        settingsMenu->shouldUpdateCommands = false;
    }
  inputBuf->Draw();
  if (inputBuf->shouldSettingsBeActive) {
      isSettingsActive = true;
      inputBuf->shouldSettingsBeActive = false;
  }
  if (settingsMenu->shouldClose) {
      isSettingsActive = false;
      settingsMenu->shouldClose = false;
  }
  if (isSettingsActive) settingsMenu->Draw();
  AppLog::Draw("Logs");
}

HRGN CreateRegionFromImGuiWindow(const char* windowName) {
    ImGuiWindow* window = ImGui::FindWindowByName(windowName);
    if (!window || window->Hidden) return nullptr;

    ImVec2 pos = window->Pos;
    ImVec2 size = window->Collapsed ? ImVec2(window->Size.x, ImGui::GetFrameHeight()) : window->Size;

    return CreateRectRgn(
        (int)pos.x, (int)pos.y, 
        (int)(pos.x + size.x), (int)(pos.y + size.y)
    );
}

HRGN App::GetCombinedWindowRegion() const {
    HRGN combined = CreateRectRgn(0, 0, 0, 0);

    if (inputBuf) {
        HRGN r1 = CreateRegionFromImGuiWindow("WSI");
        if (r1) {
            CombineRgn(combined, combined, r1, RGN_OR);
            DeleteObject(r1);
        }
    }

    if (settingsMenu && isSettingsActive) {
        HRGN r2 = CreateRegionFromImGuiWindow("Settings");
        if (r2) {
            CombineRgn(combined, combined, r2, RGN_OR);
            DeleteObject(r2);
        }
    }

    if (ENABLE_APP_LOG) {
        HRGN r3 = CreateRegionFromImGuiWindow("Logs");
        if (r3) {
            CombineRgn(combined, combined, r3, RGN_OR);
            DeleteObject(r3);
        }
    }

    return combined;
}