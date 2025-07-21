#ifndef APP_H
#define APP_H

#include <memory>
#include <windows.h>

class SettingsMenu;
class InputBuf;

class App {
    public:
        std::unique_ptr<InputBuf> inputBuf;
        std::unique_ptr<SettingsMenu> settingsMenu;
        App(const int width, const int height, const int setWidth, const int setHeight);
        bool done = false;
        bool active = false;
        void Run();
        bool isSettingsActive = false;
        HRGN GetCombinedWindowRegion() const;
};

#endif //APP_H
