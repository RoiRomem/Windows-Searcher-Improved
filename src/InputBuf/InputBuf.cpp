#include "InputBuf.h"

#include <iostream>

#include "../App/App.h"

std::string InputBuf::GetBuffer()
{
    return (std::string)inputBuf;
}

std::string ToLower(const std::string &input)
{
    std::string output;
    output.reserve(input.size());
    for (char c : input)
    {
        output += std::tolower(c);
    }
    return output;
}

std::string doubleToCleanString(double val)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(10) << val; // enough precision

    std::string str = out.str();

    // Remove trailing zeros
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);

    // If last char is '.', remove it too (means integer)
    if (!str.empty() && str.back() == '.')
    {
        str.pop_back();
    }

    // If string is empty now (could happen if val was 0)
    if (str.empty())
    {
        return "0";
    }

    return str;
}

void InputBuf::Draw()
{
    auto *viewport = ImGui::GetMainViewport();
    ImVec2 center = viewport->GetCenter();
    ImVec2 size = ImVec2(windowSize[0], windowSize[1]);
    ImVec2 pos = ImVec2(center.x - size.x * 0.5f, center.y - size.y * 0.5f);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    // Push window padding style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, colorMap["background"]);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, colorMap["frame"]);
    ImGui::PushStyleColor(ImGuiCol_Text, colorMap["text"]);

    ImGui::Begin("WSI", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoSavedSettings);

    ImGui::PushItemWidth(size.x - 2 * padding);

    if (needsInitialFocus)
    {
        ImGui::SetKeyboardFocusHere();
        ImGui::SetWindowFocus();
        needsInitialFocus = false;
    }

    if (forceClearOnNextFrame)
    {
        memset(inputBuf, 0, sizeof(inputBuf));
        oldInputBuf.clear();
        forceClearOnNextFrame = false;
    }

    if (ImGui::InputText("##input", inputBuf, sizeof(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        // here we will handle Activating ts
        if (currentFinding.first.empty() && currentFinding.second.empty())
        {
            if (isMathValidExpression(GetBuffer()))
            {
                CopyToClipboard(doubleToCleanString(evaluateExpression(GetBuffer())));
            }
            else if (!GetBuffer().empty())
            {
                Runner::RunAction(string_to_wstring(GetBuffer()));
            }
        }
        else
        {
            if (!GetBuffer().empty() && !currentFinding.second.empty())
            {
                Runner::RunAction(currentFinding.second);
            }
            else if (!currentFinding.first.empty())
            {
                if (currentFinding.first == L"exit")
                {
                    app->done = true;
                }
                else if (currentFinding.first == L"settings")
                {
                    shouldSettingsBeActive = true;
                }
                else
                {
                    Runner::RunAction(currentFinding.first);
                }
            }
        }
        inputBuf[0] = 0;
    }
    InputLogic();

    ImGui::PopItemWidth();

    ImGui::End();

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    oldInputBuf = GetBuffer();
}

std::wstring GetExtension(std::wstring str)
{
    return str.substr(str.length() >= 4 ? str.length() - 4 : 0);
}

void InputBuf::ForceFocus()
{
    needsInitialFocus = true;
}

/*
 *  sorts findings based on
 */
std::vector<std::pair<std::wstring, std::wstring>> InputBuf::SortFindings(std::vector<std::pair<std::wstring, std::wstring>> findings) const
{
    auto returnVector = std::vector<std::pair<std::wstring, std::wstring>>();

    for (const auto &ext : priorityList)
    {
        auto it = findings.begin();
        while (it != findings.end())
        {
            if (GetExtension(it->second) == ext)
            {
                returnVector.push_back(*it);
                it = findings.erase(it); // erase returns the next valid iterator
            }
            else
            {
                ++it;
            }
        }
    }

    for (const auto &remaining : findings)
    {
        returnVector.push_back(remaining);
    }

    return returnVector;
}

bool printed = false;

void InputBuf::InputLogic()
{
    // checks if the buffer had changed
    if (strcmp(inputBuf, oldInputBuf.c_str()))
    {
        cacheFind->UpdateInput(inputBuf);
        currentIndex = 0;
        const auto cache = cacheFind->GetAllItems();
    }

    const auto findings = cacheFind->GetClosestMatchesSimple(string_to_wstring(GetBuffer()), 4);
    const auto findings_sorted = SortFindings(findings);
    if (currentIndex >= findings_sorted.size())
        currentIndex = 0;

    ImGui::BeginChild("results_scroll", ImVec2(0, 0), false);

    int i = 0;
    for (const auto &match : findings_sorted)
    {
        bool is_selected = (i == currentIndex);

        if (match.second.empty())
        {
            const std::string text = "Run Command: " + wstring_to_string(match.first);
            if (ImGui::Selectable(text.c_str(), is_selected))
            {
                if (currentIndex == i)
                    Runner::RunAction(match.second);
                else
                    currentIndex = i;
            }
        }
        else
        {
            const std::string text = wstring_to_string(match.first);
            if (!match.second.empty())
            {
                if (ImGui::Selectable(text.c_str(), is_selected))
                {
                    if (currentIndex == i)
                        Runner::RunAction(match.second);
                    else
                        currentIndex = i;
                }
            }
        }

        // Set current finding for the selected item
        if (is_selected)
        {
            currentFinding = match;
        }

        ++i;
    }

    if (i == 0 && !std::string(inputBuf).empty())
    {
        currentFinding = std::pair<std::wstring, std::wstring>(L"", L"");
        if (isMathValidExpression(GetBuffer()))
            ImGui::Selectable(doubleToCleanString(evaluateExpression(GetBuffer())).c_str());
        else if (Runner::looksLikeUrl(string_to_wstring(GetBuffer())))
        {
            if (ImGui::Selectable(("Goto website: " + std::string(inputBuf)).c_str(), true))
                if (!GetBuffer().empty())
                    Runner::RunAction(string_to_wstring(GetBuffer()));
            ;
        }
        else
        {
            if (ImGui::Selectable(("Search: " + std::string(inputBuf)).c_str(), true))
                if (!GetBuffer().empty())
                    Runner::RunAction(string_to_wstring(GetBuffer()));
        }
    }
    ImGui::EndChild();
}

void InputBuf::UpdateConfig(std::unordered_map<std::string, ImVec4> config)
{
    colorMap = config;
}