#pragma once

#include "API/ARK/Ark.h"

DECLARE_HOOK(APrimalCharacter_CanDragCharacter, bool, APrimalCharacter*, APrimalCharacter*);
DECLARE_HOOK(APrimalCharacter_OnBeginDrag, void, APrimalCharacter*, APrimalCharacter*, int, bool);
DECLARE_HOOK(UPrimalInventoryComponent_RemoteInventoryAllowViewing, bool, UPrimalInventoryComponent*, AShooterPlayerController*);
DECLARE_HOOK(AShooterGameMode_StartNewShooterPlayer, void, AShooterGameMode*, APlayerController*, bool, bool, FPrimalPlayerCharacterConfigStruct*, UPrimalPlayerData*);
DECLARE_HOOK(AShooterCharacter_ControllerLeavingGame, void, AShooterCharacter*, AShooterPlayerController*);
DECLARE_HOOK(AShooterCharacter_Die, bool, AShooterCharacter*, float, FDamageEvent*, AController*, AActor*);
DECLARE_HOOK(AShooterCharacter_Destroyed, void, AShooterCharacter*);