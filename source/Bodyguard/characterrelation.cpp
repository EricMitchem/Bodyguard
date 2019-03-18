// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include "actor.hpp"
#include "characterrelation.hpp"

bool GetPlayerSubjectRelation(AShooterCharacter* actor, APrimalCharacter* subject, CharacterRelation& relation)
{
    if(actor == nullptr || subject == nullptr) {
        // To Do: log
        return false;
    }

    const auto actor_id = ArkApi::GetApiUtils().GetPlayerID(actor);
    const auto actor_team = actor->TargetingTeamField();
    const auto subject_team = subject->TargetingTeamField();

    if(IsPlayer(subject)) {
        const auto subject_id = ArkApi::GetApiUtils().GetPlayerID(subject);

        relation.is_owner = actor_id == subject_id;
    }
    else if(IsDino(subject)) {
        const auto subject_owner_id = AsDino(subject)->OwningPlayerIDField();

        relation.is_owner = subject_owner_id ? actor_id == subject_owner_id : actor_team == subject_team;
    }
    else {
        // To Do: log
        return false;
    }

    relation.is_tribe = actor_team == subject_team;
    relation.is_alliance = ArkApi::GetApiUtils().GetShooterGameMode()->AreTribesAllied(actor_team, subject_team);
    return true;
}