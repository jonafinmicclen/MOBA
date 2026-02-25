#pragma once

#include "Networking/Packets/PacketBase.hpp"
#include "Networking/Packets/PacketFactory.hpp"

#include <memory>

template<typename Derived, PacketType Type>
class AutoRegisterPacket : public virtual PacketBase {
public:
    static void register_pkt() {
        PacketFactory::registerPacket(
            Type,
            [] {return std::make_unique<Derived>();}
        );
    }
    static void ensureRegistered() {
        (void)registered;
    }
private:
    static inline bool registered = [] {
        PacketFactory::registerPacket(
            Type,
            [] { return std::make_unique<Derived>(); }
        );
        return true;
    }();
};
