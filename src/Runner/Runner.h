#ifndef RUNNER_H
#define RUNNER_H

#include <string>
#include <sstream>
#include <unordered_set>
#include <regex>
#include <windows.h>
#include <locale>
#include <cwctype>
#include <unordered_map>

#define DDG_SEARCH_URL L"duckduckgo.com/?t=ffab&q="

inline std::wstring wstrlwr(const std::wstring& input) {
    std::wstring out = input;
    std::locale loc;

    for (wchar_t& ch : out)
        ch = std::towlower(ch);

    return out;
}

namespace Runner {
    void RunAction(std::wstring action);
    bool looksLikeCommand(std::wstring input);
    bool looksLikeUrl(std::wstring input);

    inline std::unordered_set<std::wstring> knownCommands = {
        L"exit",L"start", L"taskkill", L"tasklist", L"ping", L"ipconfig", L"netstat",
        L"shutdown", L"cmd", L"restart", L"settings"
    };

    inline std::unordered_map<std::wstring, std::wstring> customCommands = {};

    inline std::unordered_set<std::wstring> knownCommandsOriginal = {
        L"exit",L"start", L"taskkill", L"tasklist", L"ping", L"ipconfig", L"netstat",
        L"shutdown", L"cmd", L"restart", L"settings"
    };
}

#endif //RUNNER_H