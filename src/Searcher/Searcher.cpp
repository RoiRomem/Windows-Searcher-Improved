#include <unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include "Searcher.h"
#include <codecvt>
#include <locale>
#include <string>
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <filesystem>

#include "../helpers/AppLogs.h"

using namespace winrt;
using namespace Windows::Management::Deployment;

inline std::optional<std::wstring> safe_getenv_w(const char *var)
{
    const char *val = std::getenv(var);
    if (!val)
        return std::nullopt;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(val);
}

std::vector<std::pair<std::wstring, std::wstring>> FindMCStoreApps()
{
    std::vector<std::pair<std::wstring, std::wstring>> results;

    try
    {
        winrt::init_apartment();
        winrt::Windows::Management::Deployment::PackageManager manager;
        auto packages = manager.FindPackagesForUser(L"");

        for (auto const &package : packages)
        {
            try
            {
                auto appEntries = package.GetAppListEntries();

                for (auto const &app : appEntries)
                {
                    auto displayInfo = app.DisplayInfo();
                    std::wstring name = displayInfo.DisplayName().c_str();
                    std::wstring aumid = app.AppUserModelId().c_str();

                    if (!name.empty())
                    {
                        results.push_back({name, L"shell:AppsFolder\\" + aumid});
                    }
                }
            }
            catch (...)
            {
                continue;
            }
        }
    }
    catch (...)
    {
        return {};
    }

    return results;
}

bool IsLinkToExe(const std::wstring &lnkPath)
{
    IShellLinkW *psl = nullptr;
    bool isExe = false;

    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID *)&psl);
    if (SUCCEEDED(hr))
    {
        IPersistFile *ppf = nullptr;
        hr = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

        if (SUCCEEDED(hr))
        {
            hr = ppf->Load(lnkPath.c_str(), STGM_READ);

            if (SUCCEEDED(hr))
            {
                wchar_t targetPath[MAX_PATH];
                hr = psl->GetPath(targetPath, MAX_PATH, NULL, SLGP_RAWPATH);

                if (SUCCEEDED(hr))
                {
                    std::wstring extension = fs::path(targetPath).extension().wstring();
                    isExe = (_wcsicmp(extension.c_str(), L".exe") == 0);
                }
            }
            ppf->Release();
        }
        psl->Release();
    }
    return isExe;
}

/**
 * Scans all directories in 'dirs' and returns a map from
 * filename without extension to the path of the file with
 * the highest priority extension found.
 *
 */
std::unordered_map<std::wstring, std::wstring> Searcher::FindInstalledApps()
{
    std::unordered_map<std::wstring, std::wstring> apps;

    for (const auto &dir : dirs)
    {
        for (const auto &entry : fs::recursive_directory_iterator(dir))
        {
            if (!entry.is_regular_file())
                continue;

            auto path = entry.path();
            auto ext = path.extension().wstring();

            if (!IsTargetExtension(ext))
                continue;

            if (ext == L".lnk" && !IsLinkToExe(path.wstring()))
            {
                std::string pathStr;
                try
                {
                    pathStr = path.string();
                }
                catch (...)
                {
                    pathStr = "unknown";
                }
                AppLog::AddLog("[INFO] Skipping " + pathStr + " because it's a .lnk that doesn't point to an .exe");
                continue;
            }

            std::wstring stem = path.stem().wstring(); // filename without extension
            int priority = GetPriority(ext);

            // if no entry or found higher priority file, update map
            auto it = apps.find(stem);
            if (it == apps.end() || priority > GetPriority(fs::path(it->second).extension().wstring()))
            {
                apps[stem] = path.wstring();
            }
        }
    }
    auto mcstoreApps = FindMCStoreApps();
    for (const auto &[name, aumid] : mcstoreApps)
    {
        auto it = apps.find(name);
        if (it == apps.end() || GetPriority(L".exe") > GetPriority(fs::path(it->second).extension().wstring()))
        {
            apps[name] = aumid;
        }
    }
    return apps;
}

/**
 * Returns the priority of an extension. Higher means better.
 * .exe = 3, .lnk = 2, .url = 1, else 0.
 */
int Searcher::GetPriority(const std::wstring &ext) const
{
    if (ext == L".exe")
        return 3;
    if (ext == L".lnk")
        return 2;
    if (ext == L".url")
        return 1;
    return 0;
}

/**
 * Checks if the extension is one of the target types.
 */
bool Searcher::IsTargetExtension(const std::wstring &ext) const
{
    return ext == L".exe" || ext == L".lnk" || ext == L".url";
}

std::vector<std::wstring> Searcher::GetDefaultSearchDirs() const
{
    std::vector<std::wstring> result;

    if (auto appdata = safe_getenv_w("APPDATA"))
    {
        result.emplace_back(fs::path(*appdata) / L"Microsoft\\Windows\\Start Menu\\Programs");
    }

    if (auto userprofile = safe_getenv_w("USERPROFILE"))
    {
        result.emplace_back(fs::path(*userprofile) / L"Desktop");
    }

    result.emplace_back(L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs");

    return result;
}
