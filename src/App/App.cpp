#include "App.h"

#include <imgui_internal.h>

#include "../InputBuf/InputBuf.h"
#include "../SettingsMenu/SettingsMenu.h"

App::App(const int width, const int height, const int setWidth, const int setHeight) {
    inputBuf = std::make_unique<InputBuf>(width, height, this);
    settingsMenu = std::make_unique<SettingsMenu>(setWidth, setHeight);
}

void App::Run() {
  if (inputBuf->getRequestedClose()) done = true;
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
}

HRGN CreateRegionFromImGuiWindow(const char* windowName) {
    ImGuiWindow* window = ImGui::FindWindowByName(windowName);
    if (!window || window->Hidden)
        return nullptr;

    ImVec2 pos = window->Pos;
    ImVec2 size = window->Size;

    return CreateRectRgn(
        static_cast<int>(pos.x),
        static_cast<int>(pos.y),
        static_cast<int>(pos.x + size.x),
        static_cast<int>(pos.y + size.y)
    );
}

// In App.cpp or wherever you handle platform/window logic
HRGN App::GetCombinedWindowRegion() const {
    HRGN combined = CreateRectRgn(0, 0, 0, 0); // Start with empty

    // Add inputBuf window
    if (inputBuf) {
        HRGN r1 = CreateRegionFromImGuiWindow("WSI");
        if (r1) {
            CombineRgn(combined, combined, r1, RGN_OR);
            DeleteObject(r1);
        }
    }

    // Add settingsMenu window if visible
    if (settingsMenu && isSettingsActive) {
        HRGN r2 = CreateRegionFromImGuiWindow("Settings");
        if (r2) {
            CombineRgn(combined, combined, r2, RGN_OR);
            DeleteObject(r2);
        }
    }

    return combined;
}