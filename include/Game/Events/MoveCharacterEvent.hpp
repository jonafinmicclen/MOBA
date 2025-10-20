// MoveCharacterEvent.hpp
#pragma once

#include "Game/Events/BaseEvent.hpp"

#include "Game/Structs/CharacterID.hpp"
#include "Common/Coordinates/WorldSpacePos.hpp"


struct MoveCharacterEvent : BaseEvent {

    EventType GetType() const override { return EventType::MoveCharacter; };

    CharacterID id;
    WorldSpacePos pos;
};

struct MoveCharacterRequest : MoveCharacterEvent {};