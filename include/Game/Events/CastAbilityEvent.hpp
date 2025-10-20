// CastAbilityEvent.hpp
#pragma once

#include "Game/Events/BaseEvent.hpp"

#include "Game/Structs/CharacterID.hpp"
#include "Game/Structs/AbilityID.hpp"
#include "Common/Coordinates/WorldSpacePos.hpp"


struct CastAbilityEvent : BaseEvent{

    EventType GetType() const override { return EventType::CastAbility; };

    CharacterID casterID;
    AbilityID ability;
    WorldSpacePos origin;
    WorldSpacePos direction;

};

struct CastAbilityRequest : CastAbilityEvent {};

