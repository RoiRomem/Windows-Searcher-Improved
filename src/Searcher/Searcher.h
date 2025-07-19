#ifndef SEARCHER_H
#define SEARCHER_H

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <optional>

namespace fs = std::filesystem;

/*
    A class that search the fs for files
*/
class Searcher {
    private:
        std::vector<std::wstring> dirs = GetDefaultSearchDirs();
        bool IsTargetExtension(const std::wstring& ext) const;
        int GetPriority(const std::wstring& ext) const;
        std::vector<std::wstring> GetDefaultSearchDirs() const;

    public:
        std::unordered_map<std::wstring, std::wstring> InstalledApps;
        std::unordered_map<std::wstring, std::wstring> FindInstalledApps();
        Searcher() : InstalledApps(FindInstalledApps()) {}
};




#endif //SEARCHER_H
