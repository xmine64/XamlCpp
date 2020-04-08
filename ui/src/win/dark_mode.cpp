#include <wil/resource.h>
#include <wil/result_macros.h>
#include <xaml/ui/win/dark_mode.h>

#include <Uxtheme.h>
#include <dwmapi.h>

using namespace std;

EXTERN_C void WINAPI RtlGetNtVersionNumbers(LPDWORD, LPDWORD, LPDWORD);

using pfShouldUseDarkMode = BOOL(WINAPI*)();
static pfShouldUseDarkMode pShouldSystemUseDarkMode;
static pfShouldUseDarkMode pShouldAppUseDarkMode;

using pfAllowDarkModeForApp = BOOL(WINAPI*)(BOOL);
static pfAllowDarkModeForApp pAllowDarkModeForApp;

using pfSetPreferredAppMode = XAML_PREFERRED_APP_MODE(WINAPI*)(XAML_PREFERRED_APP_MODE);
static pfSetPreferredAppMode pSetPreferredAppMode;

using pfIsDarkModeAllowedForApp = BOOL(WINAPI*)();
static pfIsDarkModeAllowedForApp pIsDarkModeAllowedForApp;

using pfFlushMenuThemes = void(WINAPI*)();
static pfFlushMenuThemes pFlushMenuThemes;

static wil::unique_hmodule uxtheme = nullptr;

static DWORD get_build_version()
{
    DWORD build;
    RtlGetNtVersionNumbers(nullptr, nullptr, &build);
    return build & ~0xF0000000;
}

void WINAPI XamlInitializeDarkModeFunc()
{
    if (!uxtheme)
    {
        uxtheme.reset(LoadLibraryEx(L"Uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
        if (uxtheme)
        {
            DWORD build = get_build_version();
            pShouldSystemUseDarkMode = (pfShouldUseDarkMode)GetProcAddress(uxtheme.get(), MAKEINTRESOURCEA(138));
            pShouldAppUseDarkMode = (pfShouldUseDarkMode)GetProcAddress(uxtheme.get(), MAKEINTRESOURCEA(132));
            if (build < 18362)
                pAllowDarkModeForApp = (pfAllowDarkModeForApp)GetProcAddress(uxtheme.get(), MAKEINTRESOURCEA(135));
            else
                pSetPreferredAppMode = (pfSetPreferredAppMode)GetProcAddress(uxtheme.get(), MAKEINTRESOURCEA(135));
            pIsDarkModeAllowedForApp = (pfIsDarkModeAllowedForApp)GetProcAddress(uxtheme.get(), MAKEINTRESOURCEA(139));
            pFlushMenuThemes = (pfFlushMenuThemes)GetProcAddress(uxtheme.get(), MAKEINTRESOURCEA(136));
        }
    }
}

BOOL WINAPI XamlShouldSystemUseDarkMode()
{
    if (pShouldSystemUseDarkMode)
    {
        return pShouldSystemUseDarkMode();
    }
    else
    {
        return XamlShouldAppUseDarkMode();
    }
}

BOOL WINAPI XamlShouldAppUseDarkMode()
{
    return pShouldAppUseDarkMode && pShouldAppUseDarkMode();
}

XAML_PREFERRED_APP_MODE WINAPI XamlSetPreferredAppMode(XAML_PREFERRED_APP_MODE value)
{
    if (pSetPreferredAppMode)
    {
        return pSetPreferredAppMode(value);
    }
    else if (pAllowDarkModeForApp)
    {
        BOOL result = pAllowDarkModeForApp(value == XAML_PREFERRED_APP_MODE_ALLOW_DARK || value == XAML_PREFERRED_APP_MODE_FORCE_DARK);
        return result ? XAML_PREFERRED_APP_MODE_ALLOW_DARK : XAML_PREFERRED_APP_MODE_DEFAULT;
    }
    return XAML_PREFERRED_APP_MODE_DEFAULT;
}

BOOL WINAPI XamlIsDarkModeAllowedForApp()
{
    return pIsDarkModeAllowedForApp && pIsDarkModeAllowedForApp();
}

BOOL WINAPI XamlIsDarkModeEnabledForApp()
{
    HIGHCONTRAST hc{};
    hc.cbSize = sizeof(hc);
    RETURN_IF_WIN32_BOOL_FALSE(SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0));
    if (hc.dwFlags & HCF_HIGHCONTRASTON) return FALSE;
    DWORD build = get_build_version();
    return (build < 18362 ? XamlShouldAppUseDarkMode() : XamlShouldSystemUseDarkMode()) && XamlIsDarkModeAllowedForApp();
}

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 0x13
#endif // !DWMWA_USE_IMMERSIVE_DARK_MODE

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE_V2
#define DWMWA_USE_IMMERSIVE_DARK_MODE_V2 0x14
#endif // !DWMWA_USE_IMMERSIVE_DARK_MODE_V2

HRESULT WINAPI XamlWindowUseDarkMode(HWND hWnd)
{
    BOOL set_dark_mode = TRUE;
    if (FAILED(DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &set_dark_mode, sizeof(BOOL))))
    {
        RETURN_IF_FAILED(DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_V2, &set_dark_mode, sizeof(BOOL)));
    }
    SendMessage(hWnd, WM_THEMECHANGED, 0, 0);
    if (pFlushMenuThemes) pFlushMenuThemes();
    return S_OK;
}

HRESULT WINAPI XamlControlUseDarkMode(HWND hWnd)
{
    return SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
}
