// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma comment(lib, "ArkApi.lib")

#include "API/ARK/Ark.h"
#include "bodydragging.hpp"
#include "config.hpp"
#include "inventoryaccess.hpp"

// Testing
#include "actor.hpp"

// To Do:
// - Command to reload config file
// - Command to write default config to file
// - Write readme file
// - Exception safety
// - RCON

// Functions of Interest
//void OnPrimalCharacterSleeped() { NativeCall<void>(this, "APrimalCharacter.OnPrimalCharacterSleeped"); }
//void OnPrimalCharacterUnsleeped() { NativeCall<void>(this, "APrimalCharacter.OnPrimalCharacterUnsleeped"); }
//void ControllerLeavingGame(AShooterPlayerController * theController) { NativeCall<void, AShooterPlayerController *>(this, "APrimalCharacter.ControllerLeavingGame", theController); }
//void ControllerLeavingGame(AShooterPlayerController * theController) { NativeCall<void, AShooterPlayerController *>(this, "AShooterCharacter.ControllerLeavingGame", theController); }
//void SetActorHiddenInGame(bool bNewHidden) { NativeCall<void, bool>(this, "AShooterCharacter.SetActorHiddenInGame", bNewHidden); }
//void AdjustDamage(float * Damage, FDamageEvent * DamageEvent, AController * EventInstigator, AActor * DamageCauser) { NativeCall<void, float *, FDamageEvent *, AController *, AActor *>(this, "APrimalCharacter.AdjustDamage", Damage, DamageEvent, EventInstigator, DamageCauser); }
//float BuffAdjustDamage(float Damage, FHitResult * HitInfo, AController * EventInstigator, AActor * DamageCauser, TSubclassOf<UDamageType> TheDamgeType) { return NativeCall<float, float, FHitResult *, AController *, AActor *, TSubclassOf<UDamageType>>(this, "APrimalBuff.BuffAdjustDamage", Damage, HitInfo, EventInstigator, DamageCauser, TheDamgeType); }

DECLARE_HOOK(APawn_ShouldTakeDamage, bool, APawn*, float, FDamageEvent*, AController*, AActor*);
DECLARE_HOOK(AActor_TakeDamage, float, AActor*, float, FDamageEvent*, AController*, AActor*);
DECLARE_HOOK(APrimalCharacter_IsInvincible, bool, APrimalCharacter*);

static bool IsInvincible = false;
static bool ShouldTakeDamage = true;

bool Hook_APawn_ShouldTakeDamage(APawn* _this, float damage, FDamageEvent* damage_event, AController* event_instigator, AActor* damage_causer)
{
    Log::GetLog()->info("Hook_APawn_ShouldTakeDamage called");

    const auto actor = damage_causer;
    const auto subject = _this;

    const auto actor_team = actor->TargetingTeamField();
    const auto subject_team = subject->TargetingTeamField();

    if(actor_team < 50000 && subject_team < 50000) {
        return APawn_ShouldTakeDamage_original(_this, damage, damage_event, event_instigator, damage_causer);
    }

    FString name;

    damage_causer->GetHumanReadableName(&name);
    Log::GetLog()->info("Actor: {}", name.ToString());

    _this->GetHumanReadableName(&name);
    Log::GetLog()->info("Subject: {}", name.ToString());

    Log::GetLog()->info("Potential Damage: {}", damage);

    if(IsPlayer(_this) && ShouldTakeDamage == false) {
        Log::GetLog()->info("Ignoring damage");
        return 0.f;
    }

    Log::GetLog()->info("Taking damage");
    return AActor_TakeDamage_original(_this, damage, damage_event, event_instigator, damage_causer);
}

float Hook_AActor_TakeDamage(AActor* _this, float damage, FDamageEvent* damage_event, AController* event_instigator, AActor* damage_causer)
{
    Log::GetLog()->info("Hook_AActor_TakeDamage called");

    const auto actor = damage_causer;
    const auto subject = _this;

    const auto actor_team = actor->TargetingTeamField();
    const auto subject_team = subject->TargetingTeamField();

    if(actor_team < 50000 && subject_team < 50000) {
        return AActor_TakeDamage_original(_this, damage, damage_event, event_instigator, damage_causer);
    }

    FString name;

    damage_causer->GetHumanReadableName(&name);
    Log::GetLog()->info("Actor: {}", name.ToString());

    _this->GetHumanReadableName(&name);
    Log::GetLog()->info("Subject: {}", name.ToString());

    Log::GetLog()->info("Potential Damage: {}", damage);

    if(IsPlayer(_this) && ShouldTakeDamage == false) {
        Log::GetLog()->info("Ignoring damage");
        return 0.f;
    }

    Log::GetLog()->info("Taking damage");
    return AActor_TakeDamage_original(_this, damage, damage_event, event_instigator, damage_causer);
}

bool Hook_APrimalCharacter_IsInvincible(APrimalCharacter* subject)
{
    const auto subject_team = subject->TargetingTeamField();

    if(subject_team < 50000) {
        return APrimalCharacter_IsInvincible_original(subject);
    }

    Log::GetLog()->info("Hook_APrimalCharacter_IsInvincible called");

    FString name;

    subject->GetDescriptiveName(&name);
    Log::GetLog()->info("Subject: {}", name.ToString());

    if(IsPlayer(subject) && IsInvincible == true) {
        return true;
    }

    return APrimalCharacter_IsInvincible_original(subject);
}

void Cmd_ToggleDamage(AShooterPlayerController* pc, FString* message, int mode)
{
    ShouldTakeDamage = ShouldTakeDamage ? false : true;
    ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Damage toggled. State: {}", ShouldTakeDamage);
}

void Cmd_ToggleInvincibility(AShooterPlayerController* pc, FString* message, int mode)
{
    IsInvincible = IsInvincible ? false : true;
    ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Invincibility toggled. State: {}", IsInvincible);
}

void Cmd_ToggleDrowning(AShooterPlayerController* pc, FString* message, int mode)
{
    static bool CanDrown = true;

    CanDrown = CanDrown ? false : true;

    ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Drowning toggled. State: {}", CanDrown);

    const auto player = AsPlayer(pc->CharacterField());
    auto status_comp = player->MyCharacterStatusComponentField();

    status_comp->bCanSuffocate() = CanDrown;
}

void Load()
{
    Log::Get().Init("Bodyguard");
    Config::ReadFromFile();

    ArkApi::GetHooks().SetHook(
        "APrimalCharacter.CanDragCharacter",
        &BodyDragging::Hook_APrimalCharacter_CanDragCharacter,
        &APrimalCharacter_CanDragCharacter_original
    );

    ArkApi::GetHooks().SetHook(
        "APrimalCharacter.OnBeginDrag",
        &BodyDragging::Hook_APrimalCharacter_OnBeginDrag,
        &APrimalCharacter_OnBeginDrag_original
    );

    ArkApi::GetHooks().SetHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &InventoryAccess::Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing,
        &UPrimalInventoryComponent_RemoteInventoryAllowViewing_original
    );

    ArkApi::GetHooks().SetHook(
        "APawn.ShouldTakeDamage",
        &Hook_APawn_ShouldTakeDamage,
        &APawn_ShouldTakeDamage_original
    );

    ArkApi::GetHooks().SetHook(
        "AActor.TakeDamage",
        &Hook_AActor_TakeDamage,
        &AActor_TakeDamage_original
    );

    ArkApi::GetHooks().SetHook(
        "APrimalCharacter.IsInvincible",
        &Hook_APrimalCharacter_IsInvincible,
        &APrimalCharacter_IsInvincible_original
    );

    ArkApi::GetCommands().AddChatCommand("/toggledamage", &Cmd_ToggleDamage);
    ArkApi::GetCommands().AddChatCommand("/toggleinvincibility", &Cmd_ToggleInvincibility);
    ArkApi::GetCommands().AddChatCommand("/toggledrowning", &Cmd_ToggleDrowning);
}

void Unload()
{
    ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.CanDragCharacter",
        &BodyDragging::Hook_APrimalCharacter_CanDragCharacter
    );

    ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.OnBeginDrag",
        &BodyDragging::Hook_APrimalCharacter_OnBeginDrag
    );

    ArkApi::GetHooks().DisableHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &InventoryAccess::Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing
    );

    ArkApi::GetHooks().DisableHook(
        "APawn.ShouldTakeDamage",
        &Hook_APawn_ShouldTakeDamage
    );

    ArkApi::GetHooks().DisableHook(
        "AActor.TakeDamage",
        &Hook_AActor_TakeDamage
    );

    ArkApi::GetHooks().DisableHook(
        "APrimalCharacter.IsInvincible",
        &Hook_APrimalCharacter_IsInvincible
    );

    ArkApi::GetCommands().RemoveChatCommand("/toggledamage");
    ArkApi::GetCommands().RemoveChatCommand("/toggleinvincibility");
    ArkApi::GetCommands().RemoveChatCommand("/toggledrowning");
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