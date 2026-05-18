// EventPacketConverter.hpp
#pragma once

#include "Game/Events/BaseEvent.hpp"
#include "Networking/Packets/Packets.hpp"

#include <memory>
#include <iostream>


class EventPacketConverter {
    public:

    template<typename PacketT, typename EventT>
    static std::unique_ptr<PacketBase> createPacket(std::unique_ptr<BaseEvent> event) {
        if (!event) {
            std::cerr << "[EventPacketConverter]: Event was nullptr.\n";
            return nullptr;
        }

        // Try dynamic_cast on the raw pointer
        EventT* derivedEvent = dynamic_cast<EventT*>(event.get());
        if (!derivedEvent) {
            std::cerr << "[EventPacketConverter]: Invalid event type for packet.\n";
            return nullptr;
        }

        // Move ownership safely into packet->event
        auto packet = std::make_unique<PacketT>();
        packet->event = std::unique_ptr<EventT>(static_cast<EventT*>(event.release())); 
        // now the packet owns the event, no double delete

        return packet;
    }
  


    inline static std::unique_ptr<PacketBase> eventToPacket(std::unique_ptr<BaseEvent> event) {
        if (!event) std::cerr << "[EventPacketConverter]: Event was nullptr.\n"; return nullptr;

        EventType type = event->GetType();
        switch (type) {
            case EventType::MoveCharacter:
                return createPacket<MoveCharacterPacket, MoveCharacterEvent>(std::move(event));
            case EventType::CastAbility:
                return createPacket<CastAbilityPacket, CastAbilityEvent>(std::move(event));
        }

        std::cerr << "[EventPacketConverter]: Attempted to convert unknown event to packet.\n";
        return nullptr;
    }

    inline static std::unique_ptr<BaseEvent> packetToEvent(std::unique_ptr<PacketBase> packet) {
        if (!packet) {
            std::cerr << "[EventPacketConverter]: Packet was nullptr.\n"; 
            return nullptr;
        }

        if (!packet->event) std::cerr << "[EventPacketConverter]: Packets' event was nullptr.\n";

        return std::move(packet->event);

    }
};