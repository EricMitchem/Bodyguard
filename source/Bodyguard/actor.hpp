#pragma once

#include "API/ARK/Ark.h"

struct AActor;
struct APrimalCharacter;
struct APrimalDinoCharacter;
struct AShooterCharacter;

bool IsCharacter(AActor* actor);
bool IsDino(AActor* actor);
bool IsPlayer(AActor* actor);
APrimalCharacter* AsCharacter(AActor* actor);
APrimalDinoCharacter* AsDino(AActor* actor);
AShooterCharacter* AsPlayer(AActor* actor);
bool IsOnPlayerTeam(AActor* actor);
//bool GetPlayerName(AShooterCharacter* player, FString& name);