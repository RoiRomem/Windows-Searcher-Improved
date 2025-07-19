#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <locale>
#include <codecvt>
#include <unordered_map>

inline std::wstring string_to_wstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

inline std::string wstring_to_string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

#endif //HELPER_H
