// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include "json.hpp"

using json = nlohmann::json;

enum class ConfigVars
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

class Config
{
public:
    static void ReadFromFile();
    static void WriteDefaultConfigToFile();

private:
    static const json Default;
    static json Impl;

    static bool IsValid(const json& config);
};