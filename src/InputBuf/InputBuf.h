#ifndef INPUTBUF_H
#define INPUTBUF_H

#include <string>
#include <memory>
#include <array>

#include "imgui.h"
#include "../Searcher/Searcher.h"
#include "../CacheFind/CacheFind.h"
#include "../helpers/helper.h"
#include "../Runner/Runner.h"

#define INPUT_BUFFER_LENGTH 256

class App; // 👈 Forward declare to avoid circular include

class InputBuf {
private:
    char inputBuf[INPUT_BUFFER_LENGTH] = "";
    std::string oldInputBuf = "";
    int windowSize[2] = {0, 0};
    std::array<std::wstring, 3> priorityList = {L".exe", L".lnk", L".url"};
    std::unique_ptr<Searcher> searcher;
    std::unique_ptr<CacheFind> cacheFind;
    void InputLogic();
    std::vector<std::pair<std::wstring, std::wstring>> SortFindings(std::vector<std::pair<std::wstring, std::wstring>> findings) const;
    int currentIndex = 0;
    std::pair<std::wstring, std::wstring> currentFinding;
    bool requestClose = false;
    long long click_index = 0;
    App* app;

public:
    ImVec4 inputBufColors = ImVec4(0.1f, 0.1f, 0.1f, 1);
    float padding = 10.0f;

    bool getRequestedClose() {
        if (requestClose) {
            const bool temp = requestClose;
            requestClose = false;
            return temp;
        }
        return false;
    }

    void RequestHide() {
        requestClose = true;
    }

    InputBuf(int width, int height, App* app) : app(app) {
        searcher = std::make_unique<Searcher>();
        cacheFind = std::make_unique<CacheFind>(searcher->InstalledApps);
        windowSize[0] = width;
        windowSize[1] = height;
    }

    std::string GetBuffer();
    void Draw();
    void IncreaseIndex() { ++currentIndex; }
    void DecreaseIndex() { --currentIndex; }
    void ClearSearch() {
        inputBuf[0] = '\0';
    }
};

#endif //INPUTBUF_H
