#include "API/ARK/Ark.h"
#include "actor.hpp"
#include "characterrelation.hpp"

std::optional<CharacterRelation> GetPlayerSubjectRelation(AShooterCharacter* actor, APrimalCharacter* subject)
{
    if(actor == nullptr) {
        Log::GetLog()->error("{}:{}: actor is null", __func__, __LINE__);
        return std::nullopt;
    }

    if(subject == nullptr) {
        Log::GetLog()->error("{}:{}: subject is null", __func__, __LINE__);
        return std::nullopt;
    }

    const auto actor_id = ArkApi::GetApiUtils().GetPlayerID(actor);
    const auto actor_team = actor->TargetingTeamField();
    const auto subject_team = subject->TargetingTeamField();
    CharacterRelation relation;

    if(IsPlayer(subject)) {
        const auto subject_id = ArkApi::GetApiUtils().GetPlayerID(subject);

        relation.is_owner = actor_id == subject_id;
    }
    else if(IsDino(subject)) {
        const auto subject_owner_id = AsDino(subject)->OwningPlayerIDField();

        relation.is_owner = subject_owner_id ? actor_id == subject_owner_id : actor_team == subject_team;
    }
    else {
        Log::GetLog()->error("{}:{}: Subject isn't a player or dino", __func__, __LINE__);
        return std::nullopt;
    }

    relation.is_tribe = actor_team == subject_team;
    relation.is_alliance = ArkApi::GetApiUtils().GetShooterGameMode()->AreTribesAllied(actor_team, subject_team);
    return relation;
}