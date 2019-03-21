// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "actor.hpp"
#include "charactertype.hpp"

bool GetCharacterType(APrimalCharacter* character, CharacterType& type)
{
    if(character == nullptr) {
        Log::GetLog()->error("GetCharacterType: character cannot be null");
        return false;
    }

    if(IsPlayer(character)) {
        const auto player = AsPlayer(character);

        if(player->bIsDead().Get() == true) {
            type = CharacterType::DeadPlayer;
        }
        else if(player->bIsSleeping().Get() == true) {
            type = CharacterType::OfflinePlayer;
        }
        else if(player->IsConscious() == false) {
            type = CharacterType::TranqdPlayer;
        }
        else {
            Log::GetLog()->warn("GetCharacterType: unknown player type");
            return false;
        }
    }
    else if(IsDino(character)) {
        type = CharacterType::DeadDino;
    }
    else {
        Log::GetLog()->warn("GetCharacterType: unknown character type");
        return false;
    }

    return true;
}