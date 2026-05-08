#include <vector>
#include <string>
#include "imgui.h"

constexpr const bool ENABLE_APP_LOG = false;

class AppLog
{
public:
    static void AddLog(std::string msg)
    {
        if (!ENABLE_APP_LOG)
            return;
        if (Items.size() > 50)
            Items.erase(Items.begin());
        Items.push_back(msg);
        ScrollToBottom = true;
    }

    static void Draw(const char *title, bool *p_open = NULL)
    {
        if (!ENABLE_APP_LOG)
            return;
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, NULL, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Clear"))
            Items.clear();
        ImGui::Separator();

        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto &item : Items)
        {
            if (item.find("[ERR]") != std::string::npos)
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), item.c_str());
            else
                ImGui::TextUnformatted(item.c_str());
        }

        if (ScrollToBottom)
            ImGui::SetScrollHereY(1.0f);

        ScrollToBottom = false;
        ImGui::Text("Log Count: %d", (int)Items.size());
        ImGui::EndChild();
        ImGui::End();
    }

private:
    inline static std::vector<std::string> Items;
    inline static bool ScrollToBottom;
};