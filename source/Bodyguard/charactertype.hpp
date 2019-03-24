// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include <optional>

struct APrimalCharacter;

enum class CharacterType
{
    DeadDino,
    DeadPlayer,
    OfflinePlayer,
    TranqdPlayer
};

std::optional<CharacterType> GetCharacterType(APrimalCharacter* subject);