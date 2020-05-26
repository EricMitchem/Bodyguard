#pragma once

#include <optional>

struct APrimalCharacter;
struct AShooterCharacter;

struct CharacterRelation
{
    bool is_owner;
    bool is_tribe;
    bool is_alliance;
};

std::optional<CharacterRelation> GetPlayerSubjectRelation(AShooterCharacter* actor, APrimalCharacter* subject);