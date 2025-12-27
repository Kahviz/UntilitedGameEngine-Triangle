#pragma once
#include <sdkddkver.h>

// Minimoi turhat Windows.h-symbolit, mutta s‰ilyt‰ tarvittavat API:t
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Pakolliset include-tiedostot ImGui:lle ja DX11 Win32 backendille
#include <windows.h>
#include <commctrl.h>   // scroll, common controls
#include <windowsx.h>   // helper macros
#include <shellapi.h>   // shell API (tarvittaessa)

// STRICT m‰‰rittelee tiukemman tyypityksen Win32-handleille
#define STRICT
