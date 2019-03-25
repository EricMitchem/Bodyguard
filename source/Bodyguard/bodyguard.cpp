// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma comment(lib, "ArkApi.lib")

#include "API/ARK/Ark.h"
#include "bodydragging.hpp"
#include "config.hpp"
#include "inventoryaccess.hpp"

// To Do:
// - Exception safety
// - Crash protection branch
// - RCON

void Cmd_ReloadConfig(APlayerController* _pc, FString* message, bool log)
{
    bool success = Config::ReadFromFile();
    const auto pc = static_cast<AShooterPlayerController*>(_pc);

    if(success == true) {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Config reloaded");
    }
    else {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Red, "Failed to reload config");
    }
}

void Cmd_WriteDefaultConfigToFile(APlayerController* _pc, FString* message, bool log)
{
    bool success = Config::WriteDefaultConfigToFile();
    const auto pc = static_cast<AShooterPlayerController*>(_pc);

    if(success == true) {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Default config written to file");
    }
    else {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Red, "Failed to write default config to file");
    }
}

void Load()
{
    Log::Get().Init("Bodyguard");
    Config::ReadFromFile();

    ArkApi::GetHooks().SetHook(
        "APrimalCharacter.CanDragCharacter",
        &BodyDragging::Hook_APrimalCharacter_CanDragCharacter,
        &APrimalCharacter_CanDragCharacter_original
    );

    ArkApi::GetHooks().SetHook(
        "APrimalCharacter.OnBeginDrag",
        &BodyDragging::Hook_APrimalCharacter_OnBeginDrag,
        &APrimalCharacter_OnBeginDrag_original
    );

    ArkApi::GetHooks().SetHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &InventoryAccess::Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing,
        &UPrimalInventoryComponent_RemoteInventoryAllowViewing_original
    );

    ArkApi::GetCommands().AddConsoleCommand("Bodyguard.ReloadConfig", &Cmd_ReloadConfig);
    ArkApi::GetCommands().AddConsoleCommand("Bodyguard.WriteDefaultConfigToFile", &Cmd_WriteDefaultConfigToFile);
}

void Unload()
{
    ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.CanDragCharacter",
        &BodyDragging::Hook_APrimalCharacter_CanDragCharacter
    );

    ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.OnBeginDrag",
        &BodyDragging::Hook_APrimalCharacter_OnBeginDrag
    );

    ArkApi::GetHooks().DisableHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &InventoryAccess::Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing
    );

    ArkApi::GetCommands().RemoveConsoleCommand("Bodyguard.ReloadConfig");
    ArkApi::GetCommands().RemoveConsoleCommand("Bodyguard.WriteDefaultConfigToFile");
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch(reason) {
    case DLL_PROCESS_ATTACH:
        Load();
        break;

    case DLL_PROCESS_DETACH:
        Unload();
        break;
    }

    return TRUE;
}