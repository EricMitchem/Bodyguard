#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "API/ARK/Ark.h"

// Functions of interest
//void ReceiveAnyDamage(float Damage, UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser) { NativeCall<void, float, UDamageType *, AController *, AActor *>(this, "AActor.ReceiveAnyDamage", Damage, DamageType, InstigatedBy, DamageCauser); }

class CrashGuard
{
public:
    static bool Load();
    static bool Unload();
    static void Tick();
    static bool HasTimedImmunity(AShooterCharacter* player);
    static bool HasPermanentImmunity(AShooterCharacter* player);

    static void Hook_AShooterGameMode_StartNewShooterPlayer(AShooterGameMode* _this, APlayerController* _pc, bool force_create_playerdata, bool from_login, FPrimalPlayerCharacterConfigStruct* config, UPrimalPlayerData* data);
    static void Hook_AShooterCharacter_ControllerLeavingGame(AShooterCharacter* _this, AShooterPlayerController* pc);
    static bool Hook_AShooterCharacter_Die(AShooterCharacter* _this, float damage, FDamageEvent* damage_event, AController* killer, AActor* damage_causer);
    static void Hook_AShooterCharacter_Destroyed(AShooterCharacter* _this);

private:
    using HookJoinImpl = std::function<void(AShooterPlayerController*)>;
    using HookLeaveImpl = std::function<void(AShooterCharacter*, AShooterPlayerController*)>;
    using ImmunityDuration = std::chrono::duration<long long>;
    using ImmunityTime = std::chrono::system_clock::time_point;

    static HookJoinImpl Hook_Join_Impl;
    static HookLeaveImpl Hook_Leave_Impl;
    static std::map<ImmunityTime, AShooterCharacter*> TI_TimeCharacterMap;
    static std::unordered_map<AShooterCharacter*, ImmunityTime> TI_CharacterTimeMap;
    static std::unordered_set<AShooterCharacter*> PI_Players;
    static ImmunityDuration OfflineImmunitySeconds;
    static ImmunityDuration ReconnectImmunitySeconds;

    static void GiveImmunity(AShooterCharacter* player);
    static void RemoveImmunity(AShooterCharacter* player);

    static void Hook_Join_TimedImmunity(AShooterPlayerController* pc);
    static void Hook_Leave_TimedImmunity(AShooterCharacter* _this, AShooterPlayerController* pc);
    static void Hook_Join_PermanentImmunity(AShooterPlayerController* pc);
    static void Hook_Leave_PermanentImmunity(AShooterCharacter* _this, AShooterPlayerController* pc);
    static void Hook_Join_CrashGuardDisabled(AShooterPlayerController* pc);
    static void Hook_Leave_CrashGuardDisabled(AShooterCharacter* _this, AShooterPlayerController* pc);
};