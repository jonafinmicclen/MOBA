#pragma once


#include "Networking/Packets/PacketBase.hpp"

template<typename eventT>
class PacketEvent : PacketBase {
    private:
    eventT event;

    public:
    virtual eventT& getEvent() {
        return event;
    }

    virtual void setEvent(eventT& newEvent) {
        event = newEvent;
    }
};