// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "API/ARK/Ark.h"

#pragma comment(lib, "ArkApi.lib")

void Load()
{
}

void Unload()
{
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