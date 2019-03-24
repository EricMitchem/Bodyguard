// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include <optional>
#include "accesslevel.hpp"
#include "charactertype.hpp"
#include "json.hpp"

using json = nlohmann::json;

class Config
{
public:
    enum class Var
    {
        Admins_DeadDinoDragging,
        Admins_DeadDinoInventory,
        Admins_DeadPlayerDragging,
        Admins_DeadPlayerInventory,
        Admins_OfflinePlayerDragging,
        Admins_OfflinePlayerInventory,
        Admins_TranqdPlayerDragging,
        Admins_TranqdPlayerInventory,
        CrashGuard_Enable,
        CrashGuard_PermanentImmunity,
        CrashGuard_OfflineImmunitySeconds,
        CrashGuard_ReconnectImmunitySeconds,
        Players_DeadDinoDragging,
        Players_DeadDinoInventory,
        Players_DeadPlayerDragging,
        Players_DeadPlayerInventory,
        Players_OfflinePlayerDragging,
        Players_OfflinePlayerInventory,
        Players_TranqdPlayerDragging,
        Players_TranqdPlayerInventory
    };

    static Var GetAdminVarForDragging(const CharacterType& type);
    static Var GetAdminVarForInventory(const CharacterType& type);
    static Var GetVarForDragging(const CharacterType& type);
    static Var GetVarForInventory(const CharacterType& type);
    static std::optional<AccessLevel> GetAccessLevelForVar(Var var);
    static std::optional<bool> GetBoolForAdminVar(Var var);
    static void ReadFromFile();
    static void WriteDefaultConfigToFile();

private:
    static const json Default;
    static json Impl;

    static bool IsValid(const json& config);
};