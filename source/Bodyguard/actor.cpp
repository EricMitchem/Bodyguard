// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "API/ARK/Ark.h"
#include "actor.hpp"

bool IsCharacter(AActor* actor)
{
    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return false;
    }

    return actor->IsA(APrimalCharacter::GetPrivateStaticClass());
}

bool IsDino(AActor* actor)
{
    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return false;
    }

    return actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass());
}

bool IsPlayer(AActor* actor)
{
    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return false;
    }

    return actor->IsA(AShooterCharacter::GetPrivateStaticClass());
}

APrimalCharacter* AsCharacter(AActor* actor)
{
    return static_cast<APrimalCharacter*>(actor);
}

APrimalDinoCharacter* AsDino(AActor* actor)
{
    return static_cast<APrimalDinoCharacter*>(actor);
}

AShooterCharacter* AsPlayer(AActor* actor)
{
    return static_cast<AShooterCharacter*>(actor);
}

bool IsOnPlayerTeam(AActor* actor)
{
    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return false;
    }

    // Conflicting information as to whether this should be 10000 or 50000.
    // I've seen more evidence of 10000; so, I'm using that.
    return actor->TargetingTeamField() >= 10000;
}

bool GetPlayerName(AShooterCharacter* player, FString& name)
{
    name = "Player";
    const auto player_data = player->GetPlayerData();

    if(player_data == nullptr) {
        Log::GetLog()->error("{}:{}: failed to get UPrimalPlayerData from AShooterCharacter", __func__, __LINE__);
        return false;
    }

    const auto player_state = player_data->GetPlayerState(nullptr, true);

    if(player_state == nullptr) {
        Log::GetLog()->error("{}:{}: failed to get AShooterPlayerState from UPrimalPlayerData", __func__, __LINE__);
        return false;
    }

    player_state->GetPlayerName(&name);
    return true;
}