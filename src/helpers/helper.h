#pragma once

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
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();

    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hGlob) {
        CloseClipboard();
        return;
    }

    void* pGlob = GlobalLock(hGlob);
    memcpy(pGlob, text.c_str(), text.size() + 1);
    GlobalUnlock(hGlob);

    SetClipboardData(CF_TEXT, hGlob);

    CloseClipboard();
}

#include <imgui.h>
#include <unordered_map>
#include <utility>
#include <Windows.h>

inline std::pair<UINT, UINT> ImGuiKeyToVKMod(ImGuiKey key) {
    // Reject keys outside valid range
    if (key < ImGuiKey_NamedKey_BEGIN || key >= ImGuiKey_NamedKey_END)
        return {0, 0};

    // Modifiers
    switch (key) {
        case ImGuiKey_LeftCtrl:  return {0, MOD_CONTROL};
        case ImGuiKey_RightCtrl: return {0, MOD_CONTROL};
        case ImGuiKey_LeftShift: return {0, MOD_SHIFT};
        case ImGuiKey_RightShift:return {0, MOD_SHIFT};
        case ImGuiKey_LeftAlt:   return {0, MOD_ALT};
        case ImGuiKey_RightAlt:  return {0, MOD_ALT};
        // ImGuiKey_LeftSuper / RightSuper aren't supported by Windows API
        default: break;
    }

    // Key map
    static const std::unordered_map<ImGuiKey, UINT> keyMap = {
        { ImGuiKey_Tab, VK_TAB }, { ImGuiKey_LeftArrow, VK_LEFT }, { ImGuiKey_RightArrow, VK_RIGHT },
        { ImGuiKey_UpArrow, VK_UP }, { ImGuiKey_DownArrow, VK_DOWN }, { ImGuiKey_PageUp, VK_PRIOR },
        { ImGuiKey_PageDown, VK_NEXT }, { ImGuiKey_Home, VK_HOME }, { ImGuiKey_End, VK_END },
        { ImGuiKey_Insert, VK_INSERT }, { ImGuiKey_Delete, VK_DELETE }, { ImGuiKey_Backspace, VK_BACK },
        { ImGuiKey_Space, VK_SPACE }, { ImGuiKey_Enter, VK_RETURN }, { ImGuiKey_Escape, VK_ESCAPE },

        // Letters
        { ImGuiKey_A, 'A' }, { ImGuiKey_B, 'B' }, { ImGuiKey_C, 'C' }, { ImGuiKey_D, 'D' },
        { ImGuiKey_E, 'E' }, { ImGuiKey_F, 'F' }, { ImGuiKey_G, 'G' }, { ImGuiKey_H, 'H' },
        { ImGuiKey_I, 'I' }, { ImGuiKey_J, 'J' }, { ImGuiKey_K, 'K' }, { ImGuiKey_L, 'L' },
        { ImGuiKey_M, 'M' }, { ImGuiKey_N, 'N' }, { ImGuiKey_O, 'O' }, { ImGuiKey_P, 'P' },
        { ImGuiKey_Q, 'Q' }, { ImGuiKey_R, 'R' }, { ImGuiKey_S, 'S' }, { ImGuiKey_T, 'T' },
        { ImGuiKey_U, 'U' }, { ImGuiKey_V, 'V' }, { ImGuiKey_W, 'W' }, { ImGuiKey_X, 'X' },
        { ImGuiKey_Y, 'Y' }, { ImGuiKey_Z, 'Z' },

        // Numbers
        { ImGuiKey_0, '0' }, { ImGuiKey_1, '1' }, { ImGuiKey_2, '2' }, { ImGuiKey_3, '3' },
        { ImGuiKey_4, '4' }, { ImGuiKey_5, '5' }, { ImGuiKey_6, '6' }, { ImGuiKey_7, '7' },
        { ImGuiKey_8, '8' }, { ImGuiKey_9, '9' },

        // Function keys
        { ImGuiKey_F1, VK_F1 }, { ImGuiKey_F2, VK_F2 }, { ImGuiKey_F3, VK_F3 }, { ImGuiKey_F4, VK_F4 },
        { ImGuiKey_F5, VK_F5 }, { ImGuiKey_F6, VK_F6 }, { ImGuiKey_F7, VK_F7 }, { ImGuiKey_F8, VK_F8 },
        { ImGuiKey_F9, VK_F9 }, { ImGuiKey_F10, VK_F10 }, { ImGuiKey_F11, VK_F11 }, { ImGuiKey_F12, VK_F12 },

        // Numpad
        { ImGuiKey_Keypad0, VK_NUMPAD0 }, { ImGuiKey_Keypad1, VK_NUMPAD1 }, { ImGuiKey_Keypad2, VK_NUMPAD2 },
        { ImGuiKey_Keypad3, VK_NUMPAD3 }, { ImGuiKey_Keypad4, VK_NUMPAD4 }, { ImGuiKey_Keypad5, VK_NUMPAD5 },
        { ImGuiKey_Keypad6, VK_NUMPAD6 }, { ImGuiKey_Keypad7, VK_NUMPAD7 }, { ImGuiKey_Keypad8, VK_NUMPAD8 },
        { ImGuiKey_Keypad9, VK_NUMPAD9 }, { ImGuiKey_KeypadEnter, VK_RETURN },

        // Symbols
        { ImGuiKey_Semicolon, VK_OEM_1 }, { ImGuiKey_Equal, VK_OEM_PLUS },
        { ImGuiKey_Comma, VK_OEM_COMMA }, { ImGuiKey_Minus, VK_OEM_MINUS },
        { ImGuiKey_Period, VK_OEM_PERIOD }, { ImGuiKey_Slash, VK_OEM_2 },
        { ImGuiKey_GraveAccent, VK_OEM_3 }, { ImGuiKey_LeftBracket, VK_OEM_4 },
        { ImGuiKey_Backslash, VK_OEM_5 }, { ImGuiKey_RightBracket, VK_OEM_6 },
        { ImGuiKey_Apostrophe, VK_OEM_7 }
    };

    auto it = keyMap.find(key);
    if (it != keyMap.end())
        return {it->second, 0};

    return {0, 0}; // Unmappable key
}
