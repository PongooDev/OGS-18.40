#include "framework.h"
#include "gamemode.h"
#include "Net.h"
#include "Tick.h"
#include "PC.h"
#include "PE.h"
#include "Misc.h"
#include "Abilities.h"

void InitConsole() {
    AllocConsole();
    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w+", stdout);
    SetConsoleTitleA("OGS 18.40 | Starting...");
    std::cout << "Welcome to OGS, Made with love by ObsessedTech!" << "\n";
}

void LoadWorld() {
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
}

void Hook() {
    GameMode::Hook();
    Net::Hook();
    Tick::Hook();
    PC::Hook();
    Abilities::Hook();

    PE::Hook();
    Misc::Hook();

    Sleep(1000);
    MH_EnableHook(MH_ALL_HOOKS);
}

DWORD Main(LPVOID) {
    InitConsole();
    MH_Initialize();
    Log("MinHook Initialised!");

    Hook();

    *(bool*)(InSDKUtils::GetImageBase() + 0x9c0af6b) = false; //GIsClient

    Sleep(1000);
    LoadWorld();

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

