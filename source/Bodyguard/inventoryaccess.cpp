// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "actor.hpp"
#include "characterrelation.hpp"
#include "charactertype.hpp"
#include "config.hpp"
#include "hooks.hpp"
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

    // ArkApi::GetCommands().AddConsoleCommand("Bodyguard.AccessInventory", &Cmd_AccessInventory);

    return true;
}

bool InventoryAccess::Unload()
{
    bool result = true;
    bool success;

    success = ArkApi::GetHooks().DisableHook(
        "UPrimalInventoryComponent.RemoteInventoryAllowViewing",
        &Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing
    );

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to disable hook: UPrimalInventoryComponent.RemoteInventoryAllowViewing", __func__, __LINE__);
        result = false;
    }

    // ArkApi::GetCommands().RemoveConsoleCommand("Bodyguard.AccessInventory");

    return result;
}

bool InventoryAccess::Hook_UPrimalInventoryComponent_RemoteInventoryAllowViewing(UPrimalInventoryComponent* _this, AShooterPlayerController* pc)
{
    // Log::GetLog()->info("Inventory hook called");

    const bool original_fn_result = UPrimalInventoryComponent_RemoteInventoryAllowViewing_original(_this, pc);

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
        return original_fn_result;
    }

    if(actor->bIsServerAdminField() == true) {
        // Log::GetLog()->info("Server admin: true");

        Config::Var config_var = Config::GetAdminVarForInventory(*subject_type_opt);
        const auto admin_override_opt = Config::GetBoolForAdminVar(config_var);

        if(admin_override_opt.has_value() == false) {
            Log::GetLog()->error("{}:{}: failed to get bool for admin config var", __func__, __LINE__);
            return original_fn_result;
        }

        if(admin_override_opt.value() == true) {
            // Log::GetLog()->info("Admin override: true");
            return true;
        }
    }

    // Respect Ark's default behavior if not an admin with override enabled
    if(original_fn_result == false) {
        // Log::GetLog()->info("Inventory: respecting ark's default behavior");
        return original_fn_result;
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

//void InventoryAccess::Cmd_AccessInventory(APlayerController* _pc, FString* message, bool log)
//{
//    const auto pc = static_cast<AShooterPlayerController*>(_pc);
//    const auto player = AsPlayer(pc->CharacterField());
//
//    if(player == nullptr) {
//        Log::GetLog()->error("{}:{}: failed to get AShooterCharacter from AShooterPlayerController", __func__, __LINE__);
//        return;
//    }
//
//    const auto actor = player->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, nullptr, 0.f, 0.f, nullptr, nullptr, false, false);
//
//    if(actor == nullptr) {
//        Log::GetLog()->info("{}:{}: failed to get aimed actor", __func__, __LINE__);
//        return;
//    }
//
//    const auto character = IsCharacter(actor) ? AsCharacter(actor) : nullptr;
//
//    if(character == nullptr) {
//        Log::GetLog()->info("{}:{}: aimed actor isn't a character", __func__, __LINE__);
//        return;
//    }
//
//    const auto inventory_comp = character->MyInventoryComponentField();
//
//    if(inventory_comp == nullptr) {
//        Log::GetLog()->info("{}:{}: failed to get UPrimalInventoryComponent from APrimalCharacter", __func__, __LINE__);
//        return;
//    }
//
//    //bool TryMultiUse(APlayerController * ForPC, int UseIndex) { return NativeCall<bool, APlayerController *, int>(this, "AActor.TryMultiUse", ForPC, UseIndex); }
//    //bool AllowManualMultiUseActivation(APlayerController * ForPC) { return NativeCall<bool, APlayerController *>(this, "AActor.AllowManualMultiUseActivation", ForPC); }
//    //void ModifyHudMultiUseLoc(FVector2D * theVec, APlayerController * PC, int index) { NativeCall<void, FVector2D *, APlayerController *, int>(this, "AActor.ModifyHudMultiUseLoc", theVec, PC, index); }
//    //BitFieldValue<bool, unsigned __int32> bBlueprintMultiUseEntries() { return { this, "AActor.bBlueprintMultiUseEntries" }; }
//    //BitFieldValue<bool, unsigned __int32> bEnableMultiUse() { return { this, "AActor.bEnableMultiUse" }; }
//    //BitFieldValue<bool, unsigned __int32> bMultiUseCenterHUD() { return { this, "AActor.bMultiUseCenterHUD" }; }
//    //void ActivateMultiUseSelection(bool bIsFromUseRelease) { NativeCall<void, bool>(this, "AShooterPlayerController.ActivateMultiUseSelection", bIsFromUseRelease); }
//    //void CancelMultiUseSelection() { NativeCall<void>(this, "AShooterPlayerController.CancelMultiUseSelection"); }
//    //void SetMultiUseWheelCategory(int Category) { NativeCall<void, int>(this, "AShooterPlayerController.SetMultiUseWheelCategory", Category); }
//    //void StartInventoryRadialSelector() { NativeCall<void>(this, "AShooterPlayerController.StartInventoryRadialSelector"); }
//    //void EndInventoryRadialSelector() { NativeCall<void>(this, "AShooterPlayerController.EndInventoryRadialSelector"); }
//    //void StartPlayerActionRadialSelector() { NativeCall<void>(this, "AShooterPlayerController.StartPlayerActionRadialSelector"); }
//    //void EndPlayerActionRadialSelector() { NativeCall<void>(this, "AShooterPlayerController.EndPlayerActionRadialSelector"); }
//    //void ServerRepeatMultiUse_Implementation(UObject * ForObject, int useIndex) { NativeCall<void, UObject *, int>(this, "AShooterPlayerController.ServerRepeatMultiUse_Implementation", ForObject, useIndex); }
//    //void ServerMultiUse_Implementation(UObject * ForObject, int useIndex) { NativeCall<void, UObject *, int>(this, "AShooterPlayerController.ServerMultiUse_Implementation", ForObject, useIndex); }
//    //void ClientDoMultiUse_Implementation(UObject * ForObject, int useIndex) { NativeCall<void, UObject *, int>(this, "AShooterPlayerController.ClientDoMultiUse_Implementation", ForObject, useIndex); }
//    //int GetCurrentMultiUseWheelCategory() { return NativeCall<int>(this, "AShooterPlayerController.GetCurrentMultiUseWheelCategory"); }
//    //void ClientDoMultiUse(UObject * ForObject, int useIndex) { NativeCall<void, UObject *, int>(this, "AShooterPlayerController.ClientDoMultiUse", ForObject, useIndex); }
//    //void ServerMultiUse(UObject * ForObject, int useIndex) { NativeCall<void, UObject *, int>(this, "AShooterPlayerController.ServerMultiUse", ForObject, useIndex); }
//    //bool TryMultiUse(APlayerController * ForPC, int UseIndex) { return NativeCall<bool, APlayerController *, int>(this, "APrimalCharacter.TryMultiUse", ForPC, UseIndex); }
//    //void ClientMultiUse(APlayerController * ForPC, int UseIndex) { NativeCall<void, APlayerController *, int>(this, "APrimalCharacter.ClientMultiUse", ForPC, UseIndex); }
//    //void TryAccessInventoryWrapper() { NativeCall<void>(this, "APrimalCharacter.TryAccessInventoryWrapper"); }
//    //bool TryAccessInventory() { return NativeCall<bool>(this, "APrimalCharacter.TryAccessInventory"); }
//    //void ClientMultiUse(APlayerController * ForPC, int UseIndex) { NativeCall<void, APlayerController *, int>(this, "AShooterCharacter.ClientMultiUse", ForPC, UseIndex); }
//    //bool TryMultiUse(APlayerController * ForPC, int UseIndex) { return NativeCall<bool, APlayerController *, int>(this, "AShooterCharacter.TryMultiUse", ForPC, UseIndex); }
//
//    Log::GetLog()->info("Attempting to view character's inventory");
//    inventory_comp->bAllowRemoteInventory() = true;
//    
//    inventory_comp->Activate(false);
//    //inventory_comp->ServerViewRemoteInventory(pc);
//    //pc->ServerActorViewRemoteInventory_Implementation(inventory_comp);
//}