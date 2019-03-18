// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include "API/ARK/Ark.h"

struct CharacterRelation
{
    bool is_owner;
    bool is_tribe;
    bool is_alliance;
};

bool GetPlayerSubjectRelation(AShooterCharacter* actor, APrimalCharacter* subject, CharacterRelation& relation);