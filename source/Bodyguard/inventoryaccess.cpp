// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "actor.hpp"
#include "characterrelation.hpp"
#include "charactertype.hpp"
#include "config.hpp"
#include "inventoryaccess.hpp"

bool InventoryAccess::Load()
{
    bool success;

    success = ArkApi::GetHooks().SetHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing,
        &UPrimalInventoryComponent_RemoteInventoryAllowViewing_original
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to set hook: UPrimalInventoryComponent.RemoteInventoryAllowViewing", __func__, __LINE__);
        return false;
    }

    return true;
}

bool InventoryAccess::Unload()
{
    bool success;

    success = ArkApi::GetHooks().DisableHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: UPrimalInventoryComponent.RemoteInventoryAllowViewing", __func__, __LINE__);
        return false;
    }

    return true;
}

bool InventoryAccess::Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing(UPrimalInventoryComponent* _this, AShooterPlayerController* pc)
{
    const bool original_fn_result = UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);

    if(original_fn_result == false) {
        return original_fn_result;
    }

    if(_this == nullptr) {
        Log::GetLog()->error("{}:{}: UPrimalInventoryComponent is null", __func__, __LINE__);
        return original_fn_result;
    }

    if(pc == nullptr) {
        Log::GetLog()->error("{}:{}: AShooterPlayerController is null", __func__, __LINE__);
        return original_fn_result;
    }

    const auto actor = AsPlayer(pc->CharacterField());

    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return original_fn_result;
    }

    const auto owner = _this->GetOwner();

    if(owner == nullptr) {
        Log::GetLog()->error("{}:{}: owner is null", __func__, __LINE__);
        return original_fn_result;
    }

    const auto subject = IsCharacter(owner) ? AsCharacter(owner) : nullptr;

    if(subject == nullptr) {
        return original_fn_result;
    }

    if(IsOnPlayerTeam(actor) == false || IsOnPlayerTeam(subject) == false) {
        return original_fn_result;
    }

    const auto subject_type_opt = GetCharacterType(subject);

    if(subject_type_opt.has_value() == false) {
        // Not an error path for this function
        return original_fn_result;
    }

    if(actor->bIsServerAdminField() == true) {
        Config::Var config_var = Config::GetAdminVarForInventory(*subject_type_opt);
        const auto admin_override_opt = Config::GetBoolForAdminVar(config_var);

        if(admin_override_opt.has_value() == false) {
            Log::GetLog()->error("{}:{}: failed to get bool for admin config var", __func__, __LINE__);
            return original_fn_result;
        }

        if(admin_override_opt.value() == true) {
            return original_fn_result;
        }
    }

    Config::Var config_var = Config::GetVarForInventory(*subject_type_opt);
    const auto access_level_opt = Config::GetAccessLevelForVar(config_var);

    if(access_level_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get access level for config var", __func__, __LINE__);
        return original_fn_result;
    }

    const auto relation_opt = GetPlayerSubjectRelation(actor, subject);

    if(relation_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get player-subject relation", __func__, __LINE__);
        return original_fn_result;
    }

    switch(*access_level_opt) {
    case AccessLevel::Owner:
        return relation_opt->is_owner;

    case AccessLevel::Tribe:
        return relation_opt->is_owner || relation_opt->is_tribe;

    case AccessLevel::Alliance:
        return relation_opt->is_owner || relation_opt->is_tribe || relation_opt->is_alliance;

    case AccessLevel::All:
    default:
        return original_fn_result;
    }
}