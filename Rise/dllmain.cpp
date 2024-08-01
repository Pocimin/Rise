bool isRunning = true;

#include <Windows.h>

void InitializeClient() {
    bool isDev = true;
    if (isDev) {
        // Init
    }
    else {
        return;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    bool isDev = true;
    if (isDev) {
        // Init
    }
    else {
        return;
    }

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InitializeClient, hModule, 0, nullptr);
    }

    return FALSE;
}

