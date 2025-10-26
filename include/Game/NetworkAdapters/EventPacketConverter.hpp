// EventPacketConverter.hpp
#pragma once

#include "Game/Events/BaseEvent.hpp"
#include "Networking/Packets/Packets.hpp"

#include <memory>
#include <iostream>


class EventPacketConverter {
    
    private:

    template<typename PacketT, typename EventT>
    inline static PacketBase* createPacket(BaseEvent* event) {
        if (!event) {
            std::cerr << "[EventPacketConverter]: Event was nullptr.\n";
            return nullptr;
        }

        EventT* derivedEvent = dynamic_cast<EventT*>(event);
        if (!derivedEvent) {
            std::cerr << "[EventPacketConverter]: Invalid event type for packet.\n";
            return nullptr;
        }

        PacketBase* packetPtr = new PacketT();
        packetPtr->event = derivedEvent; 

        return packetPtr;
    }

    public:

    inline static PacketBase* eventToPacket(BaseEvent* event) {
        if (!event) {
            std::cerr << "[EventPacketConverter]: Event was nullptr.\n"; 
            return nullptr;
        }

        EventType type = event->GetType();
        switch (type) {
            case EventType::MoveCharacter:
                return createPacket<MoveCharacterPacket, MoveCharacterEvent>(event);
            case EventType::CastAbility:
                return createPacket<CastAbilityPacket, CastAbilityEvent>(event);
        }

        std::cerr << "[EventPacketConverter]: Attempted to convert unknown event to packet.\n";
        return nullptr;
    }

    inline static BaseEvent* packetToEvent(PacketBase* packet) {

        std::dynamic_cast<

        if (!packet->event) std::cerr << "[EventPacketConverter]: Packets' event was nullptr.\n";

        return packet->event;

    }
};