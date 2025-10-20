#pragma once


enum class EventType {
    CastAbility = 0,
    MoveCharacter = 1
};

struct BaseEvent {
    virtual ~BaseEvent() = default;
    virtual EventType GetType() const = 0; // every event tells what type it is
};