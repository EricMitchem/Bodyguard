// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "actor.hpp"
#include "charactertype.hpp"

bool GetCharacterType(APrimalCharacter* character, CharacterType& type)
{
    if(character == nullptr) {
        // To Do: log
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
            // To Do: log
            return false;
        }
    }
    else if(IsDino(character)) {
        type = CharacterType::DeadDino;
    }
    else {
        // To Do: log
        return false;
    }

    return true;
}