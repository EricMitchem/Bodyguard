// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "API/ARK/Ark.h"
#include "accesslevel.hpp"
#include "actor.hpp"
#include "characterrelation.hpp"
#include "charactertype.hpp"
#include "config.hpp"

#pragma comment(lib, "ArkApi.lib")

// To Do: config reloading
// To Do: Write readme file
// To Do: Logging  uniformity

DECLARE_HOOK(APrimalCharacter_CanDragCharacter, bool, APrimalCharacter*, APrimalCharacter*);
DECLARE_HOOK(APrimalCharacter_OnBeginDrag, void, APrimalCharacter*, APrimalCharacter*, int, bool);
DECLARE_HOOK(UPrimalInventoryComponent_RemoteInventoryAllowViewing, bool, UPrimalInventoryComponent*, AShooterPlayerController*);

bool Hook_APrimalCharacter_CanDragCharacter(APrimalCharacter* actor, APrimalCharacter* subject)
{
#if defined(BODYGUARD_DEBUG)
    FString actor_descname, subject_descname;

    actor->GetDescriptiveName(&actor_descname);
    subject->GetDescriptiveName(&subject_descname);

    Log::GetLog()->info("--------------------Hook_APrimalCharacter_CanDragCharacter--------------------");
    Log::GetLog()->info("Actor: {}\tSubject:{}", actor_descname.ToString(), subject_descname.ToString());
#endif

    const auto actor_team = actor->TargetingTeamField();
    const auto subject_team = subject->TargetingTeamField();

#if defined(BODYGUARD_DEBUG)
    Log::GetLog()->info("Actor Team: {}\tSubject Team: {}", actor_team, subject_team);
#endif

    if(actor_team < 50000 || subject_team < 50000) {
        return APrimalCharacter_CanDragCharacter_original(actor, subject);
    }

    CharacterType subject_type;

    if(GetCharacterType(subject, subject_type) == false) {
        Log::GetLog()->warn("Failed to get character type for subject");
        return APrimalCharacter_CanDragCharacter_original(actor, subject);
    }

#if defined(BODYGUARD_DEBUG)
    Log::GetLog()->info("Subject Type: {}",
        subject_type == CharacterType::DeadDino      ? "DeadDino"      :
        subject_type == CharacterType::DeadPlayer    ? "DeadPlayer"    :
        subject_type == CharacterType::OfflinePlayer ? "OfflinePlayer" :
        subject_type == CharacterType::TranqdPlayer  ? "TranqdPlayer"  :
        "Error"
    );
#endif

    AShooterCharacter* actual_actor;

    if(IsPlayer(actor)) {
        actual_actor = AsPlayer(actor);
    }
    else if(IsDino(actor)) {
        actual_actor = AsDino(actor)->RiderField().Get();

        if(actual_actor == nullptr) {
            Log::GetLog()->warn("Failed to get rider for dino actor");
            return APrimalCharacter_CanDragCharacter_original(actor, subject);
        }
    }
    else {
        Log::GetLog()->warn("Failed to get actor type");
        return APrimalCharacter_CanDragCharacter_original(actor, subject);
    }

    if(actual_actor->bIsServerAdminField() == true) {
        Log::GetLog()->info("Server admin: true");
        Config::Var config_var = Config::GetAdminVarForDragging(subject_type);
        bool admin_override;

        if(Config::GetBoolForAdminVar(config_var, admin_override) == false) {
            Log::GetLog()->error("Failed to get bool for admin config var");
            return APrimalCharacter_CanDragCharacter_original(actor, subject);
        }

        Log::GetLog()->info("Admin override: {}", admin_override);

        if(admin_override == true) {
            return APrimalCharacter_CanDragCharacter_original(actor, subject);
        }
    }

    Config::Var config_var = Config::GetVarForDragging(subject_type);

#if defined(BODYGUARD_DEBUG)
    Log::GetLog()->info("Config Var: {}",
        config_var == Config::Var::Players_DeadDinoDragging      ? "DeadDinoDragging"      :
        config_var == Config::Var::Players_DeadPlayerDragging    ? "DeadPlayerDragging"    :
        config_var == Config::Var::Players_OfflinePlayerDragging ? "OfflinePlayerDragging" :
        config_var == Config::Var::Players_TranqdPlayerDragging  ? "TranqdPlayerDragging"  :
        "Error"
    );
#endif

    AccessLevel access_level;

    if(Config::GetAccessLevelForVar(config_var, access_level) == false) {
        Log::GetLog()->warn("Failed to get access level for config var");
        return APrimalCharacter_CanDragCharacter_original(actor, subject);
    }

#if defined(BODYGUARD_DEBUG)
    Log::GetLog()->info("Access Level: {}", AccessLevelToString(access_level));
#endif

    CharacterRelation relation;

    if(GetPlayerSubjectRelation(actual_actor, subject, relation) == false) {
        Log::GetLog()->warn("Failed to get player-subject relation");
        return APrimalCharacter_CanDragCharacter_original(actor, subject);
    }

#if defined(BODYGUARD_DEBUG)
    Log::GetLog()->info("Relation: owner: {}\ttribe: {}\talliance: {}", relation.is_owner, relation.is_tribe, relation.is_alliance);
#endif

    switch(access_level) {
    case AccessLevel::Owner:
        return relation.is_owner && APrimalCharacter_CanDragCharacter_original(actor, subject);

    case AccessLevel::Tribe:
        return (relation.is_owner || relation.is_tribe) && APrimalCharacter_CanDragCharacter_original(actor, subject);

    case AccessLevel::Alliance:
        return (relation.is_owner || relation.is_tribe || relation.is_alliance) && APrimalCharacter_CanDragCharacter_original(actor, subject);

    case AccessLevel::All:
    default:
        return APrimalCharacter_CanDragCharacter_original(actor, subject);
    }
}

// To Do: Debug logging
void Hook_APrimalCharacter_OnBeginDrag(APrimalCharacter* actor, APrimalCharacter* subject, int bone_idx, bool grap_hook)
{
    if(grap_hook == false) {
        return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
    }

    const auto actor_team = actor->TargetingTeamField();
    const auto subject_team = subject->TargetingTeamField();

    if(actor_team < 50000 || subject_team < 50000) {
        return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
    }

    CharacterType subject_type;

    if(GetCharacterType(subject, subject_type) == false) {
        Log::GetLog()->warn("Failed to get character type for subject");
        return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
    }

    if(IsPlayer(actor) == false) {
        Log::GetLog()->warn("Unknown actor using grappling hook. Using default implementation");
        return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
    }

    const auto player = AsPlayer(actor);

    if(player->bIsServerAdminField() == true) {
        Log::GetLog()->info("Server admin: true");
        Config::Var config_var = Config::GetAdminVarForDragging(subject_type);
        bool admin_override;

        if(Config::GetBoolForAdminVar(config_var, admin_override) == false) {
            Log::GetLog()->error("Failed to get bool for admin config var");
            return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
        }

        Log::GetLog()->info("Admin override: {}", admin_override);

        if(admin_override == true) {
            return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
        }
    }

    Config::Var config_var = Config::GetVarForDragging(subject_type);

    AccessLevel access_level;

    if(Config::GetAccessLevelForVar(config_var, access_level) == false) {
        Log::GetLog()->warn("Failed to get access level for config var");
        return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
    }

    CharacterRelation relation;

    if(GetPlayerSubjectRelation(player, subject, relation) == false) {
        Log::GetLog()->warn("Failed to get player-subject relation");
        return APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
    }

    bool relation_qualifies = false;

    switch(access_level) {
    case AccessLevel::Owner:
        relation_qualifies = relation.is_owner;
        break;

    case AccessLevel::Tribe:
        relation_qualifies = relation.is_owner || relation.is_tribe;
        break;

    case AccessLevel::Alliance:
        relation_qualifies = relation.is_owner || relation.is_tribe || relation.is_alliance;
        break;

    case AccessLevel::All:
        relation_qualifies = true;
        break;
    }

    if(relation_qualifies == true) {
        APrimalCharacter_OnBeginDrag_original(actor, subject, bone_idx, grap_hook);
    }
    else {
        player->DetachGrapHookCable();
    }
}

// To Do: Debug logging
bool Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing(UPrimalInventoryComponent* _this, AShooterPlayerController* pc)
{
    const auto actor = AsPlayer(pc->CharacterField());

    if(actor == nullptr) {
        Log::GetLog()->error("Failed to retrieve actor for inventory access");
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }

    const auto owner = _this->GetOwner();

    if(owner == nullptr) {
        Log::GetLog()->error("Failed to retrieve owner for inventory access");
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }

    const auto subject = IsCharacter(owner) ? AsCharacter(owner) : nullptr;

    if(subject == nullptr) {
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }

    const auto actor_team = actor->TargetingTeamField();
    const auto subject_team = subject->TargetingTeamField();

    if(actor_team < 50000 || subject_team < 50000) {
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }

    CharacterType subject_type;

    if(GetCharacterType(subject, subject_type) == false) {
        // Don't log. This hook is triggered when a player accesses his own inventory.
        // Of course that player isn't dead, offline, or tranqd (or a dino).
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }

    if(actor->bIsServerAdminField() == true) {
        Log::GetLog()->info("Server admin: true");
        Config::Var config_var = Config::GetAdminVarForInventory(subject_type);
        bool admin_override;

        if(Config::GetBoolForAdminVar(config_var, admin_override) == false) {
            Log::GetLog()->error("Failed to get bool for admin config var");
            return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
        }

        Log::GetLog()->info("Admin override: {}", admin_override);

        if(admin_override == true) {
            return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
        }
    }

    Config::Var config_var = Config::GetVarForInventory(subject_type);

    AccessLevel access_level;

    if(Config::GetAccessLevelForVar(config_var, access_level) == false) {
        Log::GetLog()->warn("Failed to get access level for config var");
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }

    CharacterRelation relation;

    if(GetPlayerSubjectRelation(actor, subject, relation) == false) {
        Log::GetLog()->warn("Failed to get player-subject relation");
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }

    switch(access_level) {
    case AccessLevel::Owner:
        return relation.is_owner && UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);

    case AccessLevel::Tribe:
        return (relation.is_owner || relation.is_tribe) && UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);

    case AccessLevel::Alliance:
        return (relation.is_owner || relation.is_tribe || relation.is_alliance) && UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);

    case AccessLevel::All:
    default:
        return UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);
    }
}

void Load()
{
    Log::Get().Init("Bodyguard");
    Config::ReadFromFile();

    ArkApi::GetHooks().SetHook(
        "APrimalCharacter.CanDragCharacter",
        &Hook_APrimalCharacter_CanDragCharacter,
        &APrimalCharacter_CanDragCharacter_original
    );

    ArkApi::GetHooks().SetHook(
        "APrimalCharacter.OnBeginDrag",
        &Hook_APrimalCharacter_OnBeginDrag,
        &APrimalCharacter_OnBeginDrag_original
    );

    ArkApi::GetHooks().SetHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing,
        &UPrimalInventoryComponent_RemoteInventoryAllowViewing_original
    );
}

void Unload()
{
    ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.CanDragCharacter",
        &Hook_APrimalCharacter_CanDragCharacter
    );

    ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.OnBeginDrag",
        &Hook_APrimalCharacter_OnBeginDrag
    );

    ArkApi::GetHooks().DisableHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing
    );
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch(reason) {
    case DLL_PROCESS_ATTACH:
        Load();
        break;

    case DLL_PROCESS_DETACH:
        Unload();
        break;
    }

    return TRUE;
}