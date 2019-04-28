// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include <functional>
#include "actor.hpp"
#include "bodydragging.hpp"
#include "characterrelation.hpp"
#include "charactertype.hpp"
#include "config.hpp"

bool BodyDragging::Load()
{
    bool success;
    
    success = ArkApi::GetHooks().SetHook(
        "APrimalCharacter.CanDragCharacter",
        &Hook_APrimalCharacter_CanDragCharacter,
        &APrimalCharacter_CanDragCharacter_original
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to set hook: APrimalCharacter.CanDragCharacter", __func__, __LINE__);
        return false;
    }

    success = ArkApi::GetHooks().SetHook(
        "APrimalCharacter.OnBeginDrag",
        &Hook_APrimalCharacter_OnBeginDrag,
        &APrimalCharacter_OnBeginDrag_original
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to set hook: APrimalCharacter.OnBeginDrag", __func__, __LINE__);
        return false;
    }

    return true;
}

bool BodyDragging::Unload()
{
    bool result = true;
    bool success;

    success = ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.CanDragCharacter",
        &Hook_APrimalCharacter_CanDragCharacter
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: APrimalCharacter.CanDragCharacter", __func__, __LINE__);
        result = false;
    }

    success = ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.OnBeginDrag",
        &Hook_APrimalCharacter_OnBeginDrag
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: APrimalCharacter.OnBeginDrag", __func__, __LINE__);
        result = false;
    }

    return result;
}

bool BodyDragging::Hook_APrimalCharacter_CanDragCharacter(APrimalCharacter* actor, APrimalCharacter* subject)
{
    const bool original_fn_result = APrimalCharacter_CanDragCharacter_original(actor, subject);

    if(original_fn_result == false) {
        return original_fn_result;
    }

    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return original_fn_result;
    }

    if(subject == nullptr) {
        Log::GetLog()->error("{}:{}: subject is null", __func__, __LINE__);
        return original_fn_result;
    }

    if(IsOnPlayerTeam(actor) == false || IsOnPlayerTeam(subject) == false) {
        return original_fn_result;
    }

    const auto subject_type_opt = GetCharacterType(subject);

    if(subject_type_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get character type for subject", __func__, __LINE__);
        return original_fn_result;
    }

    AShooterCharacter* actual_actor;

    if(IsPlayer(actor)) {
        actual_actor = AsPlayer(actor);
    }
    else if(IsDino(actor)) {
        actual_actor = AsDino(actor)->RiderField().Get();

        if(actual_actor == nullptr) {
            Log::GetLog()->error("{}:{}: failed to get rider for dino actor", __func__, __LINE__);
            return original_fn_result;
        }
    }
    else {
        Log::GetLog()->warn("{}:{}: actor isn't a player or dino", __func__, __LINE__);
        return original_fn_result;
    }

    if(actual_actor->bIsServerAdminField() == true) {
        Config::Var config_var = Config::GetAdminVarForDragging(*subject_type_opt);
        const auto admin_override_opt = Config::GetBoolForAdminVar(config_var);

        if(admin_override_opt.has_value() == false) {
            Log::GetLog()->error("{}:{}: failed to get bool for admin config var", __func__, __LINE__);
            return original_fn_result;
        }

        if(admin_override_opt.value() == true) {
            return original_fn_result;
        }
    }

    Config::Var config_var = Config::GetVarForDragging(*subject_type_opt);
    const auto access_level_opt = Config::GetAccessLevelForVar(config_var);

    if(access_level_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get access level for config var", __func__, __LINE__);
        return original_fn_result;
    }

    const auto relation_opt = GetPlayerSubjectRelation(actual_actor, subject);

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

void BodyDragging::Hook_APrimalCharacter_OnBeginDrag(APrimalCharacter* actor, APrimalCharacter* subject, int bone_idx, bool grap_hook)
{
    const auto original_fn = std::bind(APrimalCharacter_OnBeginDrag_original, actor, subject, bone_idx, grap_hook);

    if(grap_hook == false) {
        return original_fn();
    }

    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return original_fn();
    }

    if(subject == nullptr) {
        Log::GetLog()->error("{}:{}: subject is null", __func__, __LINE__);
        return original_fn();
    }

    if(IsOnPlayerTeam(actor) == false || IsOnPlayerTeam(subject) == false) {
        return original_fn();
    }

    const auto subject_type_opt = GetCharacterType(subject);

    if(subject_type_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get character type for subject", __func__, __LINE__);
        return original_fn();
    }

    if(IsPlayer(actor) == false) {
        Log::GetLog()->warn("{}:{}: non-player actor using grappling hook", __func__, __LINE__);
        return original_fn();
    }

    const auto player = AsPlayer(actor);

    if(player->bIsServerAdminField() == true) {
        Config::Var config_var = Config::GetAdminVarForDragging(*subject_type_opt);
        const auto admin_override_opt = Config::GetBoolForAdminVar(config_var);

        if(admin_override_opt.has_value() == false) {
            Log::GetLog()->error("{}:{}: failed to get bool for admin config var", __func__, __LINE__);
            return original_fn();
        }

        if(admin_override_opt.value() == true) {
            return original_fn();
        }
    }

    Config::Var config_var = Config::GetVarForDragging(*subject_type_opt);
    const auto access_level_opt = Config::GetAccessLevelForVar(config_var);

    if(access_level_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get access level for config var", __func__, __LINE__);
        return original_fn();
    }

    const auto relation_opt = GetPlayerSubjectRelation(player, subject);

    if(relation_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get player-subject relation", __func__, __LINE__);
        return original_fn();
    }

    bool relation_qualifies = false;

    switch(*access_level_opt) {
    case AccessLevel::Owner:
        relation_qualifies = relation_opt->is_owner;
        break;

    case AccessLevel::Tribe:
        relation_qualifies = relation_opt->is_owner || relation_opt->is_tribe;
        break;

    case AccessLevel::Alliance:
        relation_qualifies = relation_opt->is_owner || relation_opt->is_tribe || relation_opt->is_alliance;
        break;

    case AccessLevel::All:
    default:
        relation_qualifies = true;
        break;
    }

    if(relation_qualifies == true) {
        original_fn();
    }
    else {
        player->DetachGrapHookCable();
    }
}