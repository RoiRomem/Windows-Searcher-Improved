/* ===========================================================================
__      __  ____    ______
/\ \  __/\ \/\  _`\ /\__  _\
\ \ \/\ \ \ \ \,\L\_\/_/\ \/
 \ \ \ \ \ \ \/_\__ \  \ \ \
  \ \ \_/ \_\ \/\ \L\ \ \_\ \__
   \ `\___x___/\ `\____\/\_____\
    '\/__//__/  \/_____/\/_____/

=========================================================================== */

#include "main.h"

#include <dwmapi.h>
#include <iostream>
#include "InputBuf/InputBuf.h"

std::unique_ptr<App> *appPtr = nullptr;

// Main code
//int main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float MainScale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY));

    int scaledWidth = static_cast<int>(WINDOW_WIDTH * MainScale);
    int scaledHeight = static_cast<int>(WINDOW_HEIGHT * MainScale);
    int scaledSettingsWidth = static_cast<int>(SETTINGS_WIDTH * MainScale);
    int scaledSettingsHeight = static_cast<int>(SETTINGS_HEIGHT * MainScale);

    // Create application window
    WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"WSI", nullptr};
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        wc.lpszClassName,
        L"",
        WS_POPUP,
        (GetSystemMetrics(SM_CXSCREEN) - scaledWidth) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - scaledHeight) / 2,
        scaledWidth, scaledHeight,
        nullptr, nullptr, wc.hInstance, nullptr);

    MARGINS margins = {-1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    auto app = std::make_unique<App>(scaledWidth, scaledHeight, scaledSettingsWidth, scaledSettingsHeight);
    appPtr = &app;

    auto config = LoadConfig();

    RegisterHotKey(hwnd, 1, config.keybindMod, config.keybindVK);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(MainScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = MainScale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // style.FontSizeBase = 20.0f;
    // io.Fonts->AddFontDefault();

    auto currentFont = io.Fonts->AddFontFromFileTTF(config.fontPath.c_str());

    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    // IM_ASSERT(font != nullptr);

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // Main loop
    while (!app->done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                app->done = true;
        }
        if (app->done)
            break;

        if (app->inputBuf->getRequestedClose())
            app->active = false;
        if (!app->active)
        {
            ShowWindow(hwnd, SW_HIDE);
            Sleep(10);
            continue;
        }
        ShowWindow(hwnd, SW_SHOW);

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        if (app->isSettingsActive)
        {
            SetWindowPos(hwnd, nullptr, 0, 0, (GetSystemMetrics(SM_CXSCREEN)), (GetSystemMetrics(SM_CYSCREEN)), SWP_NOZORDER);
            auto region = app->GetCombinedWindowRegion();
            SetWindowRgn(hwnd, region, TRUE);
            DeleteObject(region);
        }
        else
        {
            SetWindowPos(hwnd, nullptr, (GetSystemMetrics(SM_CXSCREEN) - scaledWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - scaledHeight) / 2, scaledWidth, scaledHeight, SWP_NOZORDER);
            auto region = app->GetCombinedWindowRegion();
            SetWindowRgn(hwnd, region, TRUE);
            DeleteObject(region);
        }

        if (app->settingsMenu->shouldUpdate)
        {
            config = LoadConfig();
            app->inputBuf->UpdateConfig(config.colors);
            io.Fonts->RemoveFont(currentFont);
            currentFont = io.Fonts->AddFontFromFileTTF(config.fontPath.c_str());
            app->settingsMenu->shouldUpdate = false;
        }

        const float clear_color_with_alpha[4] = {
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w};

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        app->Run();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (app->settingsMenu->shouldUpdate)
        {
            config = LoadConfig();
            UnregisterHotKey(hwnd, 1);
            RegisterHotKey(hwnd, 1, config.keybindMod, config.keybindVK);
        }

        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    UnregisterHotKey(hwnd, 1);
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D *pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

bool IsAnotherAppInExclusiveFullscreen(HWND myHwnd)
{
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == myHwnd)
        return false;

    // Check if it's a real visible window
    if (!IsWindowVisible(foreground))
        return false;

    // Avoid taskbar, explorer etc
    DWORD pid;
    GetWindowThreadProcessId(foreground, &pid);
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    wchar_t exeName[MAX_PATH] = {};
    DWORD len = MAX_PATH;
    if (hProc)
    {
        QueryFullProcessImageNameW(hProc, 0, exeName, &len);
        CloseHandle(hProc);
        std::wstring path = exeName;
        if (path.find(L"explorer.exe") != std::wstring::npos ||
            path.find(L"dwm.exe") != std::wstring::npos)
            return false;
    }

    RECT winRect;
    GetWindowRect(foreground, &winRect);

    HMONITOR hMon = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monInfo = {sizeof(monInfo)};
    GetMonitorInfo(hMon, &monInfo);

    // If the window exactly covers the monitor, it’s likely fullscreen
    if (EqualRect(&winRect, &monInfo.rcMonitor))
    {
        // Possible exclusive fullscreen
        return true;
    }

    return false;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = static_cast<UINT>(LOWORD(lParam)); // Queue resize
        g_ResizeHeight = static_cast<UINT>(HIWORD(lParam));
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;

    case WM_KILLFOCUS:
        (*appPtr)->active = false;
        break;

    case WM_KEYDOWN:
        if (wParam == VK_DOWN)
            (*appPtr)->inputBuf->IncreaseIndex();
        else if (wParam == VK_UP)
            (*appPtr)->inputBuf->DecreaseIndex();
        else if (wParam == VK_ESCAPE)
        {
            (*appPtr)->active = false;
            (*appPtr)->inputBuf->ClearSearch();
        }
        break;

    case WM_HOTKEY:
        if (wParam == 0)
            break;
        if (IsAnotherAppInExclusiveFullscreen(hWnd))
            break;
        (*appPtr)->inputBuf->ClearSearch();
        (*appPtr)->active = !(*appPtr)->active;
        if ((*appPtr)->active)
        {
            (*appPtr)->inputBuf->ForceFocus();
            
            DWORD fgThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
            DWORD thisThread = GetCurrentThreadId();
            AttachThreadInput(fgThread, thisThread, TRUE);

            SetForegroundWindow(hWnd);
            SetFocus(hWnd);

            AttachThreadInput(fgThread, thisThread, FALSE);
        }
        break;
    default:;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}