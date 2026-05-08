#include "SettingsMenu.h"
#include <cstring>
#include <imgui_internal.h>

SettingsMenu::SettingsMenu(int width, int height)
    : width(width), height(height)
{
    auto cnf = LoadConfig();
    shouldUpdateCommands = true;
    frameColor = cnf.colors["frame"];
    backgroundColor = cnf.colors["background"];
    textColor = cnf.colors["text"];

    strncpy_s(fontPath, cnf.fontPath.c_str(), sizeof(fontPath));

    keybindVK = cnf.keybindVK;
    keybindMod = cnf.keybindMod;
    
    keybindKey = ImGuiKey_None;
    for (int n = ImGuiKey_NamedKey_BEGIN; n < ImGuiKey_NamedKey_END; n++) {
        auto [vk, mod] = ImGuiKeyToVKMod((ImGuiKey)n);
        if (vk == (UINT)keybindVK) {
            keybindKey = (ImGuiKey)n;
            break;
        }
    }

    customs = cnf.customCommands;

    editBuffers.clear();
    for (const auto &cmd : customs)
    {
        CommandBuffer buf;
        auto first_str = wstring_to_string(cmd.first);
        auto second_str = wstring_to_string(cmd.second);

        strncpy_s(buf.prompt, first_str.c_str(), BUFFER_MIN);
        strncpy_s(buf.command, second_str.c_str(), BUFFER_MIN);
        editBuffers.push_back(buf);
    }
}

void SettingsMenu::Draw()
{
    ImGui::SetNextWindowSize(ImVec2((float)width, (float)height), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));

    if (!ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    if (ImGui::BeginTabBar("SettingsTabs"))
    {
        if (ImGui::BeginTabItem("Appearance"))
        {
            ImGui::Spacing();
            ImGui::TextDisabled("Theme Colors");

            if (ImGui::BeginTable("ColorsTable", 2, ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableNextColumn(); ImGui::Text("Frame");
                ImGui::TableNextColumn(); ImGui::ColorEdit4("##frame", (float *)&frameColor, ImGuiColorEditFlags_NoInputs);

                ImGui::TableNextColumn(); ImGui::Text("Text");
                ImGui::TableNextColumn(); ImGui::ColorEdit4("##text", (float *)&textColor, ImGuiColorEditFlags_NoInputs);

                ImGui::TableNextColumn(); ImGui::Text("Background");
                ImGui::TableNextColumn(); ImGui::ColorEdit4("##bg", (float *)&backgroundColor, ImGuiColorEditFlags_NoInputs);

                ImGui::EndTable();
            }

            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextDisabled("Resources");
            ImGui::InputText("Font Path", fontPath, sizeof(fontPath));

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Hotkeys"))
        {
            ImGui::Spacing();
            ImGui::TextWrapped("Set the global shortcut to toggle this menu.");
            ImGui::Spacing();

            const char *modLabel = (keybindMod == MOD_CONTROL ? "Ctrl" : keybindMod == MOD_SHIFT ? "Shift"
                                     : keybindMod == MOD_ALT     ? "Alt"
                                     : "None");
            const char *vkLabel = (keybindKey != ImGuiKey_None) ? ImGui::GetKeyName(keybindKey) : "None";

            auto PollAndBind = [&](const char *mode, auto &&commit)
            {
                for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k)
                {
                    ImGuiKey imguiKey = static_cast<ImGuiKey>(k);
                    if (!ImGui::IsNamedKey(imguiKey)) continue;
                    
                    if (ImGui::IsKeyPressed(imguiKey))
                    {
                        auto [vk, mod] = ImGuiKeyToVKMod(imguiKey);
                        commit(imguiKey, vk, mod);
                        bindWaiting.clear();
                        break;
                    }
                }
            };

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Modifier:");
            ImGui::SameLine();
            if (bindWaiting == "mod")
            {
                ImGui::Button("Waiting...##mod", ImVec2(80, 0));
                PollAndBind("mod", [&](ImGuiKey k, UINT vk, UINT mod) { if(mod) keybindMod = mod; });
            }
            else
            {
                if (ImGui::Button(modLabel, ImVec2(80, 0))) bindWaiting = "mod";
            }

            ImGui::SameLine(0, 20);

            ImGui::Text("Key:");
            ImGui::SameLine();
            if (bindWaiting == "vk")
            {
                ImGui::Button("Waiting...##vk", ImVec2(80, 0));
                PollAndBind("vk", [&](ImGuiKey k, UINT vk, UINT mod) { if(vk && mod == 0) { keybindKey = k; keybindVK = vk; } });
            }
            else
            {
                if (ImGui::Button(vkLabel, ImVec2(80, 0))) bindWaiting = "vk";
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Commands"))
        {
            ImGui::Spacing();
            static ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
            
            if (ImGui::BeginTable("CommandsTable", 3, tableFlags, ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 1.5f)))
            {
                ImGui::TableSetupColumn("Prompt", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Action / Command", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("##Delete", ImGuiTableColumnFlags_WidthFixed, 30.0f);
                ImGui::TableHeadersRow();

                for (int i = 0; i < (int)editBuffers.size(); ++i)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::InputText(("##p" + std::to_string(i)).c_str(), editBuffers[i].prompt, BUFFER_MIN);

                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::InputText(("##c" + std::to_string(i)).c_str(), editBuffers[i].command, BUFFER_MIN);

                    ImGui::TableNextColumn();
                    if (ImGui::Button(("X##" + std::to_string(i)).c_str(), ImVec2(25, 0)))
                    {
                        editBuffers.erase(editBuffers.begin() + i);
                    }
                }
                ImGui::EndTable();
            }

            if (ImGui::Button("+ Add New Command", ImVec2(-FLT_MIN, 0)))
            {
                editBuffers.push_back(CommandBuffer{"", ""});
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float widthNeeded = 200.0f; 
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - widthNeeded - 15);

    if (ImGui::Button("Save Changes", ImVec2(100, 0)))
    {
        customs.clear();
        for (const auto &buf : editBuffers)
        {
            customs.emplace_back(string_to_wstring(buf.prompt), string_to_wstring(buf.command));
        }
        SaveSettings();
    }
    ImGui::SameLine();
    if (ImGui::Button("Close", ImVec2(80, 0)))
    {
        shouldClose = true;
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void SettingsMenu::SaveSettings()
{
    auto nConfig = AppConfig();
    auto oConfig = LoadConfig();

    nConfig.colors["frame"] = frameColor;
    nConfig.colors["background"] = backgroundColor;
    nConfig.colors["text"] = textColor;
    nConfig.fontPath = fontPath;

    if (keybindMod != oConfig.keybindMod || keybindVK != oConfig.keybindVK)
    {
        shouldUpdateBinds = true;
    }

    nConfig.keybindVK = keybindVK;
    nConfig.keybindMod = keybindMod;
    nConfig.customCommands = customs;

    SaveConfig(nConfig);
    shouldUpdate = true;
    shouldUpdateCommands = true;
}