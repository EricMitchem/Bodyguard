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