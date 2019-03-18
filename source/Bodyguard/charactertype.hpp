// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include "API/ARK/Actor.h"

enum class CharacterType
{
    DeadDino,
    DeadPlayer,
    OfflinePlayer,
    TranqdPlayer
};

bool GetCharacterType(APrimalCharacter* character, CharacterType& type);