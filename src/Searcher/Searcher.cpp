#include "Searcher.h"

inline std::optional<std::wstring> safe_getenv_w(const char* var) {
    const char* val = std::getenv(var);
    if (!val) return std::nullopt;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(val);
}

/**
     * Scans all directories in 'dirs' and returns a map from
     * filename without extension to the path of the file with
     * the highest priority extension found.
     *
 */
std::unordered_map<std::wstring, std::wstring> Searcher::FindInstalledApps() {
    std::unordered_map<std::wstring, std::wstring> apps;

    for (const auto& dir : dirs) {
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;

            auto path = entry.path();
            auto ext = path.extension().wstring();

            if (!IsTargetExtension(ext))
                continue;

            auto stem = path.stem().wstring();  // filename without extension
            int priority = GetPriority(ext);

            // If no entry or found higher priority file, update map
            auto it = apps.find(stem);
            if (it == apps.end() || priority > GetPriority(fs::path(it->second).extension().wstring())) {
                apps[stem] = path.wstring();
            }
        }
    }
    return apps;
}

/**
     * Returns the priority of an extension. Higher means better.
     * .exe = 3, .lnk = 2, .url = 1, else 0.
     */
int Searcher::GetPriority(const std::wstring& ext) const {
    if (ext == L".exe") return 3;
    if (ext == L".lnk") return 2;
    if (ext == L".url") return 1;
    return 0;
}

/**
     * Checks if the extension is one of the target types.
     */
bool Searcher::IsTargetExtension(const std::wstring& ext) const {
    return ext == L".exe" || ext == L".lnk" || ext == L".url";
}

std::vector<std::wstring> Searcher::GetDefaultSearchDirs() const {
    std::vector<std::wstring> result;

    if (auto appdata = safe_getenv_w("APPDATA")) {
        result.emplace_back(fs::path(*appdata) / L"Microsoft\\Windows\\Start Menu\\Programs");
    }

    if (auto userprofile = safe_getenv_w("USERPROFILE")) {
        result.emplace_back(fs::path(*userprofile) / L"Desktop");
    }

    result.emplace_back(L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs");

    return result;
}

