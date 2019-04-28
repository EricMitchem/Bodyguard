// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include "API/ARK/Ark.h"

DECLARE_HOOK(APrimalCharacter_CanDragCharacter, bool, APrimalCharacter*, APrimalCharacter*);
DECLARE_HOOK(APrimalCharacter_OnBeginDrag, void, APrimalCharacter*, APrimalCharacter*, int, bool);

class BodyDragging
{
public:
    static bool Load();
    static bool Unload();
    static bool Hook_APrimalCharacter_CanDragCharacter(APrimalCharacter* actor, APrimalCharacter* subject);
    static void Hook_APrimalCharacter_OnBeginDrag(APrimalCharacter* actor, APrimalCharacter* subject, int bone_idx, bool grap_hook);
};
