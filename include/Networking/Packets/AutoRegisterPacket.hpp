#pragma once

#include "Networking/Packets/PacketBase.hpp"
#include "Networking/Packets/PacketFactory.hpp"

#include <memory>

template<typename Derived, PacketType Type>
class AutoRegisterPacket : public PacketBase {
private:
    static inline bool registered = [] {
        PacketFactory::registerPacket(
            Type,
            [] { return std::make_unique<Derived>(); }
        );
        return true;
    }();
};
