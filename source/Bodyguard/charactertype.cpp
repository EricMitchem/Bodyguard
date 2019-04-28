// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "API/ARK/Ark.h"
#include "actor.hpp"
#include "charactertype.hpp"

std::optional<CharacterType> GetCharacterType(APrimalCharacter* subject)
{
    if(subject == nullptr) {
        Log::GetLog()->error("{}:{}: subject is null", __func__, __LINE__);
        return std::nullopt;
    }

    CharacterType subject_type;

    if(IsPlayer(subject)) {
        const auto player = AsPlayer(subject);

        if(player->bIsDead()() == true) {
            subject_type = CharacterType::DeadPlayer;
        }
        else if(player->bIsSleeping()() == true) {
            subject_type = CharacterType::OfflinePlayer;
        }
        else if(player->IsConscious() == false) {
            subject_type = CharacterType::TranqdPlayer;
        }
        else {
            return std::nullopt;
        }
    }
    else if(IsDino(subject)) {
        const auto dino = AsDino(subject);

        if(dino->bIsDead()() == true) {
            subject_type = CharacterType::DeadDino;
        }
        else {
            return std::nullopt;
        }
    }
    else {
        Log::GetLog()->error("{}:{}: subject isn't a player or dino", __func__, __LINE__);
        return std::nullopt;
    }

    return subject_type;
}