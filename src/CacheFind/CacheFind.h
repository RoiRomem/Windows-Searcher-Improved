#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

class CacheFind {
private:
    std::vector<std::unordered_map<std::wstring, std::wstring>> cacheStack;
    std::wstring currentInput;

    // Full dataset in wstring
    std::unordered_map<std::wstring, std::wstring> allItems;

    std::unordered_map<std::wstring, std::wstring> filterItems(
        const std::unordered_map<std::wstring, std::wstring>& sourceMap,
        const std::wstring& prefix);

    void AddLetter(const std::wstring& prefix);  // prefix in wstring now
    void RemoveLetter();
    void ResetCache();

public:
    std::unordered_map<std::wstring, std::wstring> CurrentMatches;
    std::vector<std::pair<std::wstring, std::wstring>> GetClosestMatches(const std::wstring& input, size_t maxResults);
    std::vector<std::pair<std::wstring, std::wstring>> GetClosestMatchesSimple(const std::wstring& input, size_t maxResults);
    explicit CacheFind(const std::unordered_map<std::wstring, std::wstring>& items);
    void UpdateCommands(const std::unordered_map<std::wstring, std::wstring>& commands);

    inline std::unordered_map<std::wstring, std::wstring> GetAllItems() const {
        return allItems;
    }

    // Input here is normal string (utf8), converted internally
    void UpdateInput(const std::string& newInput);
};
