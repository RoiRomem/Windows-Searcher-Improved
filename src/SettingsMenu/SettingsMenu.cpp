#include "SettingsMenu.h"
#include <cstring>

SettingsMenu::SettingsMenu(int width, int height)
    : width(width), height(height) {

    auto cnf = LoadConfig();
    shouldUpdateCommands = true;
    frameColor = cnf.colors["frame"];
    backgroundColor = cnf.colors["background"];
    textColor = cnf.colors["text"];

    // Copy font path safely into buffer
    strncpy(fontPath, cnf.fontPath.c_str(), sizeof(fontPath));
    fontPath[sizeof(fontPath) - 1] = '\0';

    keybindVK = cnf.keybindVK;
    keybindMod = cnf.keybindMod;

    customs = cnf.customCommands;

    if (editBuffers.size() != customs.size()) {
        editBuffers.resize(customs.size());
        for (size_t i = 0; i < customs.size(); ++i) {
            auto first_str = wstring_to_string(customs[i].first);
            auto second_str = wstring_to_string(customs[i].second);
            if (first_str.length() < BUFFER_MIN && second_str.length() < BUFFER_MIN) {
                strcpy_s(editBuffers[i].first, BUFFER_MIN, first_str.c_str());
                strcpy_s(editBuffers[i].second, BUFFER_MIN, second_str.c_str());
            }
        }
    }
}

void SettingsMenu::Draw() {
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::ColorEdit4("Frame Color", reinterpret_cast<float*>(&frameColor));
    ImGui::ColorEdit4("Text Color", reinterpret_cast<float*>(&textColor));
    ImGui::ColorEdit4("Background Color", reinterpret_cast<float*>(&backgroundColor));
    ImGui::InputText("Font (path)", fontPath, BUFFER_MIN);

    ImGui::Separator();
    ImGui::Text("Keybindings");

    const char* modLabel = (keybindMod == MOD_CONTROL ? "Ctrl"
                         : keybindMod == MOD_SHIFT   ? "Shift"
                         : keybindMod == MOD_ALT     ? "Alt"
                         :                            "None");
    const char* vkLabel = (keybindKey != ImGuiKey_None)
                            ? ImGui::GetKeyName(keybindKey)
                            : "None";

    auto PollAndBind = [&](const char* mode, auto&& commit){
        for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k) {
            ImGuiKey imguiKey = static_cast<ImGuiKey>(k);
            if (!ImGui::IsKeyPressed(imguiKey))
                continue;
            auto [vk, mod] = ImGuiKeyToVKMod(imguiKey);
            commit(imguiKey, vk, mod);
            bindWaiting.clear();
            break;
        }
    };

    // — Modifier bind —
    if (bindWaiting == "mod") {
        if (ImGui::Button("Press mod...##modBtn")) {}
        PollAndBind("mod", [&](ImGuiKey imguiKey, UINT vk, UINT mod){
            if (mod) {
                keybindMod  = mod;
            }
        });
    } else {
        if (ImGui::Button((std::string(modLabel) + "##modBtn").c_str()))
            bindWaiting = "mod";
    }

    ImGui::SameLine();

    // — VK bind —
    if (bindWaiting == "vk") {
        if (ImGui::Button("Press key...##vkBtn")) {}
        PollAndBind("vk", [&](ImGuiKey imguiKey, UINT vk, UINT mod){
            if (vk && mod == 0) {
                keybindKey  = imguiKey;
                keybindVK   = vk;
            }
        });
    } else {
        if (ImGui::Button((std::string(vkLabel) + "##vkBtn").c_str()))
            bindWaiting = "vk";
    }


    ImGui::Text("Custom Commands:");

    // Custom commands
    for (int i = 0; i < customs.size(); ++i) {
        auto& [fst, snd] = editBuffers[i];

        float fullWidth = ImGui::GetContentRegionAvail().x;

        ImGui::PushItemWidth(fullWidth * 0.3f);
        ImGui::InputText(("##prompt" + std::to_string(i)).c_str(), fst, BUFFER_MIN);
        ImGui::SameLine();
        ImGui::PushItemWidth(fullWidth * 0.65f);
        ImGui::InputText(("##command" + std::to_string(i)).c_str(), snd, BUFFER_MIN);
    }


    if (ImGui::Button("new command")) {
        customs.emplace_back();
        editBuffers.emplace_back();
        strcpy_s(editBuffers.back().first, BUFFER_MIN, "");
        strcpy_s(editBuffers.back().second, BUFFER_MIN, "");
    }

    ImGui::Separator();
    if (ImGui::Button("Save")) {
        customs.clear();
        for (auto& [prompt, cmd] : editBuffers) {
            customs.emplace_back(string_to_wstring(prompt), string_to_wstring(cmd));
        }
        SaveSettings();
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        shouldClose = true;
    }

    ImGui::End();
}

void SettingsMenu::SaveSettings() {
    auto nConfig = AppConfig();
    auto oConfig = LoadConfig();
    nConfig.colors["frame"] = frameColor;
    nConfig.colors["background"] = backgroundColor;
    nConfig.colors["text"] = textColor;
    nConfig.fontPath = fontPath;

    if (keybindMod != oConfig.keybindMod || keybindVK != oConfig.keybindVK) {
        shouldUpdateBinds = true;
    }

    nConfig.keybindVK = keybindVK;
    nConfig.keybindMod = keybindMod;

    nConfig.customCommands = customs;

    SaveConfig(nConfig);
    shouldUpdate = true;
    shouldUpdateCommands = true;
}