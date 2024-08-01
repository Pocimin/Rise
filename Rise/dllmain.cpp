// dllmain.cpp : Defines the entry point for the DLL application.
bool isRunning = true; // isRunning bool for ejection/detachment

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
    if (ul_reason_for_call == DLL_PROCESS_DETACH) {
    }
    return TRUE;
}

