// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include "API/ARK/Ark.h"

DECLARE_HOOK(UPrimalInventoryComponent_RemoteInventoryAllowViewing, bool, UPrimalInventoryComponent*, AShooterPlayerController*);

class InventoryAccess
{
public:
    static bool Load();
    static bool Unload();
    static bool Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing(UPrimalInventoryComponent* _this, AShooterPlayerController* pc);
};