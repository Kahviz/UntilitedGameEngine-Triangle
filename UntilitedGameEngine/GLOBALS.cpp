#include "Globals.h"
#include "Includer.h"
#include <cstdlib>   // _dupenv_s
#include <string>
#include <iostream>
#include <Windows.h>

int screen_width = GetSystemMetrics(SM_CXSCREEN);
int screen_height = GetSystemMetrics(SM_CYSCREEN);

std::string GetAppDataPath() {
    char* buffer = nullptr;
    size_t size = 0;

    if (_dupenv_s(&buffer, &size, "APPDATA") == 0 && buffer != nullptr) {
        std::string path(buffer);
        free(buffer);  // vapautetaan allokoitu muisti
        return path;
    }
    return "";
}

std::string appData = GetAppDataPath(); 
std::string assets = appData + "\\UntilitedGameEngine\\Assets";

