#include "Runner.h"

bool looksLikeFile(std::wstring input) {
    if (input.empty()) return false;

    return input.find(L'\\') !=std::string::npos;
}

bool Runner::looksLikeCommand(std::wstring input) {
    if (input.empty()) return false;

    static const std::unordered_set<std::wstring> knownCommands = {
        L"exit",L"start", L"taskkill", L"tasklist", L"ping", L"ipconfig", L"netstat",
        L"shutdown", L"cmd", L"restart", L"settings"
    };

    std::wistringstream iss(input);
    std::wstring firstWord;
    iss >> firstWord;

    return knownCommands.find(firstWord) != knownCommands.end();
}

bool Runner::looksLikeUrl(std::wstring input) {
    static const std::wregex urlPattern(
        LR"((https?:\/\/)?([\w\-]+\.)+[a-zA-Z]{2,}(\/\S*)?)",
        std::regex::icase
    );


    return std::regex_search(input, urlPattern);
}

// Opens file or URL
void OpenWithShell(const std::wstring& target, bool asAdmin = false) {
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = asAdmin ? L"runas" : L"open";
    sei.lpFile = target.c_str();
    sei.nShow = SW_SHOWNORMAL;

    ShellExecuteExW(&sei);
}

// Runs a command in CMD
void RunCommand(const std::wstring& cmd, bool asAdmin = false) {
    std::wstring fullCmd = L"cmd.exe /C" + cmd;

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = asAdmin ? L"runas" : L"open";
    sei.lpFile = L"cmd.exe";
    sei.lpParameters = (L"/C " + cmd).c_str();
    sei.nShow = SW_SHOWNORMAL;

    ShellExecuteExW(&sei);
}

// Open default browser with a URL
void OpenInBrowser(const std::wstring& url) {
    std::wstring finalURL = url;
    if (url.find(L"http") == std::wstring::npos)
        finalURL = L"https://" + url;

    ShellExecuteW(nullptr, L"open", finalURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}



void Runner::RunAction(std::wstring action) {
    if (looksLikeFile(action)) {
        OpenWithShell(action);
    } else if (looksLikeCommand(action)) {
        if (wstrlwr(action) == L"shutdown") RunCommand(L"shutdown /s /t 1", true);
        else if (wstrlwr(action) == L"restart") RunCommand(L"restart /s /t 1", true);
        else if (wstrlwr(action) == L"cmd") RunCommand(L"start wt cmd", true);
        else RunCommand(action);
    } else if (looksLikeUrl(action)) {
        OpenInBrowser(action);
    } else {
        OpenInBrowser(DDG_SEARCH_URL + action);
    }
}