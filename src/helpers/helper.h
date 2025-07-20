#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <locale>
#include <codecvt>
#include <unordered_map>
#include <windows.h>

inline std::wstring string_to_wstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

inline std::string wstring_to_string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

inline void CopyToClipboard(const std::string& text) {
    if (!OpenClipboard(nullptr)) return;            // open clipboard
    EmptyClipboard();                               // clear it

    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hGlob) {
        CloseClipboard();
        return;
    }

    void* pGlob = GlobalLock(hGlob);
    memcpy(pGlob, text.c_str(), text.size() + 1);
    GlobalUnlock(hGlob);

    SetClipboardData(CF_TEXT, hGlob);               // set clipboard data

    CloseClipboard();                               // close clipboard
}

#endif //HELPER_H
