#include "CacheFind.h"
#include <locale>
#include <codecvt>
#include <algorithm>
#include <queue>

std::wstring ToLower(const std::wstring& input) {
    std::wstring output;
    output.reserve(input.size());
    for (wchar_t c : input) {
        output += std::towlower(c);
    }
    return output;
}

std::wstring RemoveWhitespace(const std::wstring& input) {
    std::wstring output;
    output.reserve(input.size());
    for (wchar_t c : input) {
        if (!std::iswspace(c)) {
            output += c;
        }
    }
    return output;
}

// Memory-efficient Levenshtein distance with early termination
int levenshtein_distance_optimized(const std::wstring& s1, const std::wstring& s2, int maxDistance = -1) {
    const size_t len1 = s1.size(), len2 = s2.size();

    if (maxDistance >= 0 && abs(static_cast<int>(len1) - static_cast<int>(len2)) > maxDistance) {
        return maxDistance + 1;
    }

    std::vector<int> prev(len2 + 1);
    std::vector<int> curr(len2 + 1);

    for (size_t j = 0; j <= len2; ++j) {
        prev[j] = j;
    }

    for (size_t i = 1; i <= len1; ++i) {
        curr[0] = i;

        for (size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            curr[j] = std::min({
                prev[j] + 1,
                curr[j - 1] + 1,
                prev[j - 1] + cost
            });
        }

        if (maxDistance >= 0) {
            int minInRow = *std::min_element(curr.begin(), curr.end());
            if (minInRow > maxDistance) {
                return maxDistance + 1;
            }
        }

        prev.swap(curr);
    }

    return prev[len2];
}

std::vector<std::pair<std::wstring, std::wstring>> CacheFind::GetClosestMatches(const std::wstring& input, size_t maxResults) {
    if (CurrentMatches.empty()) {
        return {};
    }

    std::priority_queue<std::pair<int, std::pair<std::wstring, std::wstring>>> heap;

    int maxDistance = std::min(static_cast<int>(input.length()) + 5, 10);
    const auto clean_input = RemoveWhitespace(ToLower(input));

    for (const auto& entry : CurrentMatches) {
        auto clean_entry = RemoveWhitespace(ToLower(entry.first));
        int distance = levenshtein_distance_optimized(clean_entry, clean_input, maxDistance);

        if (distance <= maxDistance) {
            if (heap.size() < maxResults) {
                heap.push({distance, entry});
            } else if (distance < heap.top().first) {
                heap.pop();
                heap.push({distance, entry});
            }
        }
    }

    std::vector<std::pair<std::wstring, std::wstring>> results;
    results.reserve(heap.size());

    while (!heap.empty()) {
        results.push_back(heap.top().second);
        heap.pop();
    }

    std::reverse(results.begin(), results.end());
    return results;
}

std::vector<std::pair<std::wstring, std::wstring>> CacheFind::GetClosestMatchesSimple(const std::wstring& input, size_t maxResults) {
    std::vector<std::pair<int, std::pair<std::wstring, std::wstring>>> scored;
    scored.reserve(std::min(CurrentMatches.size(), maxResults * 3));

    const auto input_lwr = RemoveWhitespace(ToLower(input));

    for (const auto& entry_unfiltered : CurrentMatches) {
        const auto entry = RemoveWhitespace(ToLower(entry_unfiltered.first));

        size_t commonPrefix = 0;
        size_t minLen = std::min(input_lwr.length(), entry.length());

        for (size_t i = 0; i < minLen; ++i) {
            if (input_lwr[i] == entry[i]) {
                commonPrefix++;
            } else {
                break;
            }
        }

        int score = -static_cast<int>(commonPrefix) * 10 +
                   abs(static_cast<int>(input_lwr.length()) - static_cast<int>(entry.length()));

        scored.push_back({score, entry_unfiltered});
    }

    if (scored.size() > maxResults) {
        std::nth_element(scored.begin(), scored.begin() + maxResults, scored.end());
        scored.resize(maxResults);
    }

    std::sort(scored.begin(), scored.end());

    std::vector<std::pair<std::wstring, std::wstring>> results;
    results.reserve(scored.size());

    for (const auto& item : scored) {
        results.push_back(item.second);
    }

    return results;
}

static std::wstring StringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

CacheFind::CacheFind(const std::unordered_map<std::wstring, std::wstring>& items)
    : allItems(items)
{
    cacheStack.reserve(100);
    CurrentMatches.clear();
}

std::unordered_map<std::wstring, std::wstring> CacheFind::filterItems(
    const std::unordered_map<std::wstring, std::wstring>& sourceMap,
    const std::wstring& prefix)
{
    std::unordered_map<std::wstring, std::wstring> filtered;
    std::wstring clean_prefix = RemoveWhitespace(ToLower(prefix));

    for (const auto& [key, val] : sourceMap) {
        if (RemoveWhitespace(ToLower(key)).compare(0, clean_prefix.size(), clean_prefix) == 0)
            filtered.emplace(key, val);
    }
    return filtered;
}

void CacheFind::AddLetter(const std::wstring& prefix) {
    const auto& sourceMap = cacheStack.empty() ? allItems : cacheStack.back();
    cacheStack.push_back(filterItems(sourceMap, prefix));
}

void CacheFind::RemoveLetter() {
    if (!cacheStack.empty())
        cacheStack.pop_back();
}

void CacheFind::ResetCache() {
    cacheStack.clear();
}

void CacheFind::UpdateInput(const std::string& newInput) {
    std::wstring wNewInput = StringToWString(newInput);

    if (wNewInput.size() > currentInput.size() &&
        wNewInput.compare(0, currentInput.size(), currentInput) == 0) {
        for (size_t i = currentInput.size(); i < wNewInput.size(); ++i) {
            AddLetter(wNewInput.substr(0, i + 1));
        }
    }
    else if (wNewInput.size() < currentInput.size() &&
             currentInput.compare(0, wNewInput.size(), wNewInput) == 0) {
        for (size_t i = currentInput.size(); i > wNewInput.size(); --i) {
            RemoveLetter();
        }
    }
    else {
        ResetCache();
        for (size_t i = 0; i < wNewInput.size(); ++i) {
            AddLetter(wNewInput.substr(0, i + 1));
        }
    }

    currentInput = std::move(wNewInput);

    if (cacheStack.empty()) {
        CurrentMatches.clear();
    } else {
        CurrentMatches = cacheStack.back();
    }
}
