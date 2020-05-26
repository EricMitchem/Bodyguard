#pragma once

#include "API/ARK/Ark.h"

class BodyDragging
{
public:
    static bool Load();
    static bool Unload();
    static bool Hook_APrimalCharacter_CanDragCharacter(APrimalCharacter* actor, APrimalCharacter* subject);
    static void Hook_APrimalCharacter_OnBeginDrag(APrimalCharacter* actor, APrimalCharacter* subject, int bone_idx, bool grap_hook);
};
