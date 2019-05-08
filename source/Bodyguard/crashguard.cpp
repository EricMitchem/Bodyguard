// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "actor.hpp"
#include "config.hpp"
#include "crashguard.hpp"
#include "hooks.hpp"

CrashGuard::HookJoinImpl CrashGuard::Hook_Join_Impl;
CrashGuard::HookLeaveImpl CrashGuard::Hook_Leave_Impl;
std::map<CrashGuard::ImmunityTime, AShooterCharacter*> CrashGuard::TI_TimeCharacterMap;
std::unordered_map<AShooterCharacter*, CrashGuard::ImmunityTime> CrashGuard::TI_CharacterTimeMap;
std::unordered_set<AShooterCharacter*> CrashGuard::PI_Players;
CrashGuard::ImmunityDuration CrashGuard::OfflineImmunitySeconds;
CrashGuard::ImmunityDuration CrashGuard::ReconnectImmunitySeconds;

bool CrashGuard::Load()
{
    bool success;

    success = ArkApi::GetHooks().SetHook(
        "AShooterGameMode.StartNewShooterPlayer",
        &Hook_AShooterGameMode_StartNewShooterPlayer,
        &AShooterGameMode_StartNewShooterPlayer_original
    );
    
    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to set hook: AShooterGameMode.StartNewShooterPlayer", __func__, __LINE__);
        return false;
    }
    
    success = ArkApi::GetHooks().SetHook(
        "AShooterCharacter.ControllerLeavingGame",
        &Hook_AShooterCharacter_ControllerLeavingGame,
        &AShooterCharacter_ControllerLeavingGame_original
    );
    
    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to set hook: AShooterCharacter.ControllerLeavingGame", __func__, __LINE__);
        return false;
    }
    
    success = ArkApi::GetHooks().SetHook(
        "AShooterCharacter.Die",
        &Hook_AShooterCharacter_Die,
        &AShooterCharacter_Die_original
    );
    
    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to set hook: AShooterCharacter.Die", __func__, __LINE__);
        return false;
    }

    success = ArkApi::GetHooks().SetHook(
        "AShooterCharacter.Destroyed",
        &Hook_AShooterCharacter_Destroyed,
        &AShooterCharacter_Destroyed_original
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to set hook: AShooterCharacter.Destroyed", __func__, __LINE__);
        return false;
    }

    const auto enable_opt = Config::GetBoolForCrashGuardVar(Config::Var::CrashGuard_Enable);
    
    if(enable_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to get CrashGuard config var", __func__, __LINE__);
        return false;
    }
    
    if(enable_opt.value() == true) {
        const auto permanent_immunity_opt = Config::GetBoolForCrashGuardVar(Config::Var::CrashGuard_PermanentImmunity);
    
        if(permanent_immunity_opt.has_value() == false) {
            Log::GetLog()->error("{}:{}: failed to get CrashGuard config var", __func__, __LINE__);
            return false;
        }
    
        if(permanent_immunity_opt.value() == true) {
            Hook_Join_Impl = Hook_Join_PermanentImmunity;
            Hook_Leave_Impl = Hook_Leave_PermanentImmunity;
        }
        else {
            const auto offline_immunity_seconds_opt = Config::GetUintForCrashGuardVar(Config::Var::CrashGuard_OfflineImmunitySeconds);
            const auto reconnect_immunity_seconds_opt = Config::GetUintForCrashGuardVar(Config::Var::CrashGuard_ReconnectImmunitySeconds);
    
            if(offline_immunity_seconds_opt.has_value() == false || reconnect_immunity_seconds_opt.has_value() == false) {
                Log::GetLog()->error("{}:{}: failed to get Crashguard config var", __func__, __LINE__);
                return false;
            }
    
            OfflineImmunitySeconds = std::chrono::seconds(offline_immunity_seconds_opt.value());
            ReconnectImmunitySeconds = std::chrono::seconds(reconnect_immunity_seconds_opt.value());
            Hook_Join_Impl = Hook_Join_TimedImmunity;
            Hook_Leave_Impl = Hook_Leave_TimedImmunity;
        }
    }
    else {
        Hook_Join_Impl = Hook_Join_CrashGuardDisabled;
        Hook_Leave_Impl = Hook_Leave_CrashGuardDisabled;
    }

    ArkApi::GetCommands().AddOnTimerCallback(FString("Bodyguard.CrashGuard.Tick"), Tick);
    
    return true;
}

bool CrashGuard::Unload()
{
    bool result = true;
    bool success;

    success = ArkApi::GetHooks().DisableHook(
        "AShooterGameMode.StartNewShooterPlayer",
        &Hook_AShooterGameMode_StartNewShooterPlayer
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: AShooterGameMode.StartNewShooterPlayer", __func__, __LINE__);
        result = false;
    }

    success = ArkApi::GetHooks().DisableHook(
        "AShooterCharacter.ControllerLeavingGame",
        &Hook_AShooterCharacter_ControllerLeavingGame
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: AShooterCharacter.ControllerLeavingGame", __func__, __LINE__);
        result = false;
    }

    success = ArkApi::GetHooks().DisableHook(
        "AShooterCharacter.Die",
        &Hook_AShooterCharacter_Die
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: AShooterCharacter.Die", __func__, __LINE__);
        result = false;
    }

    success = ArkApi::GetHooks().DisableHook(
        "AShooterCharacter.Destroyed",
        &Hook_AShooterCharacter_Destroyed
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: AShooterCharacter.Destroyed", __func__, __LINE__);
        result = false;
    }

    Hook_Join_Impl = Hook_Join_CrashGuardDisabled;
    Hook_Leave_Impl = Hook_Leave_CrashGuardDisabled;

    for(const auto& [player, _] : TI_CharacterTimeMap) {
        RemoveImmunity(player);
    }

    TI_CharacterTimeMap.clear();
    TI_TimeCharacterMap.clear();

    for(const auto& player : PI_Players) {
        RemoveImmunity(player);
    }

    PI_Players.clear();

    ArkApi::GetCommands().RemoveOnTimerCallback(FString("Bodyguard.CrashGuard.Tick"));

    return result;
}

void CrashGuard::Tick()
{
    const auto time_now = std::chrono::system_clock::now();

    while(TI_TimeCharacterMap.empty() == false) {
        const auto it = TI_TimeCharacterMap.cbegin();
        const auto exp_time = it->first;
        const auto player = it->second;

        if(exp_time > time_now) {
            break;
        }

        RemoveImmunity(player);
        TI_TimeCharacterMap.erase(exp_time);
        TI_CharacterTimeMap.erase(player);

        // To Do: Immunity begin message
        /*const auto pc = ArkApi::GetApiUtils().FindControllerFromCharacter(player);

        if(pc != nullptr) {
            ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Your immunity has ended");
        }*/
    }
}

bool CrashGuard::HasTimedImmunity(AShooterCharacter* player)
{
    return TI_CharacterTimeMap.count(player) == 1;
}

bool CrashGuard::HasPermanentImmunity(AShooterCharacter* player)
{
    return PI_Players.count(player) == 1;
}

void CrashGuard::Hook_AShooterGameMode_StartNewShooterPlayer(AShooterGameMode* _this, APlayerController* _pc, bool force_create_playerdata, bool from_login, FPrimalPlayerCharacterConfigStruct* config, UPrimalPlayerData* data)
{
    AShooterGameMode_StartNewShooterPlayer_original(_this, _pc, force_create_playerdata, from_login, config, data);

    const auto pc = static_cast<AShooterPlayerController*>(_pc);

    if(pc == nullptr) {
        Log::GetLog()->error("{}:{}: AShooterPlayerController is null", __func__, __LINE__);
        return;
    }

    if(from_login == false) {
        return;
    }

    Hook_Join_Impl(pc);
}

void CrashGuard::Hook_AShooterCharacter_ControllerLeavingGame(AShooterCharacter* _this, AShooterPlayerController* pc)
{
    AShooterCharacter_ControllerLeavingGame_original(_this, pc);

    if(pc == nullptr) {
        Log::GetLog()->error("{}:{}: AShooterPlayerController is null", __func__, __LINE__);
        return;
    }

    Hook_Leave_Impl(_this, pc);
}

bool CrashGuard::Hook_AShooterCharacter_Die(AShooterCharacter* _this, float damage, FDamageEvent* damage_event, AController* killer, AActor* damage_causer)
{
    if(HasTimedImmunity(_this) == true) {
        const auto exp_time = TI_CharacterTimeMap.at(_this);
        TI_TimeCharacterMap.erase(exp_time);
        TI_CharacterTimeMap.erase(_this);
    }
    else if(HasPermanentImmunity(_this) == true) {
        PI_Players.erase(_this);
    }

    return AShooterCharacter_Die_original(_this, damage, damage_event, killer, damage_causer);
}

void CrashGuard::Hook_AShooterCharacter_Destroyed(AShooterCharacter* _this)
{
    if(HasTimedImmunity(_this) == true) {
        const auto exp_time = TI_CharacterTimeMap.at(_this);
        TI_TimeCharacterMap.erase(exp_time);
        TI_CharacterTimeMap.erase(_this);
    }
    else if(HasPermanentImmunity(_this) == true) {
        PI_Players.erase(_this);
    }

    AShooterCharacter_Destroyed_original(_this);
}

void CrashGuard::GiveImmunity(AShooterCharacter* player)
{
    player->bCanBeDamaged() = false;
    player->bCanBeTorpid() = false;

    auto status_comp = player->MyCharacterStatusComponentField();

    if(status_comp == nullptr) {
        Log::GetLog()->error("{}:{}: failed to get UPrimalCharacterStatusComponent from AShooterCharacter", __func__, __LINE__);
        Log::GetLog()->error("{}:{}: failed to establish drowning immunity for player", __func__, __LINE__);
        return;
    }

    status_comp->bCanSuffocate() = false;
}

void CrashGuard::RemoveImmunity(AShooterCharacter* player)
{
    player->bCanBeDamaged() = true;
    player->bCanBeTorpid() = true;

    auto status_comp = player->MyCharacterStatusComponentField();

    if(status_comp == nullptr) {
        Log::GetLog()->error("{}:{}: failed to get UPrimalCharacterStatusComponent from AShooterCharacter", __func__, __LINE__);
        Log::GetLog()->error("{}:{}: failed to remove drowning immunity for player", __func__, __LINE__);
        return;
    }

    status_comp->bCanSuffocate() = true;
}

void CrashGuard::Hook_Join_TimedImmunity(AShooterPlayerController* pc)
{
    const auto player = AsPlayer(pc->CharacterField());

    if(player == nullptr) {
        return;
    }

    if(HasTimedImmunity(player) == true) {
        const auto exp_time = TI_CharacterTimeMap.at(player);
        TI_TimeCharacterMap.erase(exp_time);
        TI_CharacterTimeMap.erase(player);
    }
    else {
        GiveImmunity(player);
    }

    const auto time = std::chrono::system_clock::now() + ReconnectImmunitySeconds;
    TI_TimeCharacterMap.insert(std::make_pair(time, player));
    TI_CharacterTimeMap.insert(std::make_pair(player, time));
}

void CrashGuard::Hook_Leave_TimedImmunity(AShooterCharacter* _this, AShooterPlayerController* pc)
{
    const auto player = _this;

    if(player == nullptr) {
        Log::GetLog()->error("{}:{}: AShooterCharacter is null", __func__, __LINE__);
        return;
    }

    if(HasTimedImmunity(player) == true) {
        const auto exp_time = TI_CharacterTimeMap.at(player);
        TI_TimeCharacterMap.erase(exp_time);
        TI_CharacterTimeMap.erase(player);
    }
    else {
        GiveImmunity(player);
    }

    const auto time = std::chrono::system_clock::now() + OfflineImmunitySeconds;
    TI_TimeCharacterMap.insert(std::make_pair(time, player));
    TI_CharacterTimeMap.insert(std::make_pair(player, time));
}

void CrashGuard::Hook_Join_PermanentImmunity(AShooterPlayerController* pc)
{
    const auto player = AsPlayer(pc->CharacterField());
    
    if(player == nullptr) {
        return;
    }

    if(HasPermanentImmunity(player) == false) {
        GiveImmunity(player);
        PI_Players.insert(player);
    }
}

void CrashGuard::Hook_Leave_PermanentImmunity(AShooterCharacter* _this, AShooterPlayerController* pc) {}

void CrashGuard::Hook_Join_CrashGuardDisabled(AShooterPlayerController* pc) {}

void CrashGuard::Hook_Leave_CrashGuardDisabled(AShooterCharacter* _this, AShooterPlayerController* pc) {}