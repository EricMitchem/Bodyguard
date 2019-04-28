// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma comment(lib, "ArkApi.lib")

#include "API/ARK/Ark.h"
#include "bodydragging.hpp"
#include "config.hpp"
#include "crashguard.hpp"
#include "inventoryaccess.hpp"

// To Do:
// - RCON

void Load()
{
    Log::Get().Init("Bodyguard");

    if(Config::Load() == false) {
        Log::GetLog()->error("{}:{}: failed to load config module", __func__, __LINE__);
        throw;
    }

    if(BodyDragging::Load() == false) {
        Log::GetLog()->error("{}:{}: failed to load body dragging module", __func__, __LINE__);
        throw;
    }

    if(InventoryAccess::Load() == false) {
        Log::GetLog()->error("{}:{}: failed to load inventory access module", __func__, __LINE__);
        throw;
    }

    // Requires Config
    if(CrashGuard::Load() == false) {
        Log::GetLog()->error("{}:{}: failed to load crash guard module", __func__, __LINE__);
        throw;
    }
}

void Unload()
{
    if(CrashGuard::Unload() == false) {
        Log::GetLog()->error("{}:{}: failed to unload crash guard module", __func__, __LINE__);
    }

    if(InventoryAccess::Unload() == false) {
        Log::GetLog()->error("{}:{}: failed to unload inventory access module", __func__, __LINE__);
    }

    if(BodyDragging::Unload() == false) {
        Log::GetLog()->error("{}:{}: failed to unload body dragging module", __func__, __LINE__);
    }

    if(Config::Unload() == false) {
        Log::GetLog()->error("{}:{}: failed to unload config module", __func__, __LINE__);
    }
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch(reason) {
    case DLL_PROCESS_ATTACH:
        try {
            Load();
        }
        catch(...) {
            return FALSE;
        }
        break;

    case DLL_PROCESS_DETACH:
        Unload();
        break;
    }

    return TRUE;
}