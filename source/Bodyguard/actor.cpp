// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "actor.hpp"

bool IsCharacter(AActor* actor)
{
    return actor ? actor->IsA(APrimalCharacter::GetPrivateStaticClass()) : false;
}

bool IsDino(AActor* actor)
{
    return actor ? actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass()) : false;
}

bool IsPlayer(AActor* actor)
{
    return actor ? actor->IsA(AShooterCharacter::GetPrivateStaticClass()) : false;
}

APrimalCharacter* AsCharacter(AActor* actor)
{
    return static_cast<APrimalCharacter*>(actor);
}

APrimalDinoCharacter* AsDino(AActor* actor)
{
    return static_cast<APrimalDinoCharacter*>(actor);
}

AShooterCharacter* AsPlayer(AActor* actor)
{
    return static_cast<AShooterCharacter*>(actor);
}