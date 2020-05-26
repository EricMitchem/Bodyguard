#pragma once

#include "API/ARK/Ark.h"

class InventoryAccess
{
public:
    static bool Load();
    static bool Unload();

    static bool Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing(UPrimalInventoryComponent* _this, AShooterPlayerController* pc);

    // static  void Cmd_AccessInventory(APlayerController* _pc, FString* message, bool log);
};