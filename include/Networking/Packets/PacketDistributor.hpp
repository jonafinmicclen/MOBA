#pragma once

#include "Networking/Packets/PacketTypes.hpp"
#include "Networking/Packets/PacketBase.hpp"
#include "Networking/Packets/PacketFactory.hpp"

#include "Networking/Packets/PacketMetadata.hpp"

#include "Debug/debug.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <stdexcept>

class PacketDistributor {
public:
    PacketDistributor() {}

    // Register a typed listener for a PacketType.
    // The listener will receive TPacket&, already cast.
    template <typename TPacket>
    void on(PacketType type, std::function<void(const TPacket&, const PacketMetadata&)> fn) {
        static_assert(std::is_base_of_v<PacketBase, TPacket>,
                      "TPacket must derive from PacketBase");

        listeners_[type].push_back(
            [type, f = std::move(fn)](const PacketBase& base, const PacketMetadata& metadata) {

                // Can remove dynamic cast later
                const auto* pkt = dynamic_cast<const TPacket*>(&base);
                if (!pkt) {
                    DEBUG_LOG("Unrecognised packet");
                    return;
                }
                f(*pkt, metadata);
            }
        );
    }

    // Dispatch packet based on packet.getType().
    // Returns true if at least one listener ran.
    bool dispatch(const PacketBase& packet, const PacketMetadata& metadata) const {
        auto it = listeners_.find(packet.getType());
        if (it == listeners_.end()) return false;

        for (const auto& cb : it->second) cb(packet, metadata);
        return !it->second.empty();
    }

    bool dispatch(const std::unique_ptr<PacketBase>& packet, const PacketMetadata& metadata) const {
        return packet ? dispatch(*packet, metadata) : false;
    }

    void clear() { listeners_.clear(); }
    void clear(PacketType type) { listeners_.erase(type); }

private:
    struct EnumHash {
        size_t operator()(PacketType t) const noexcept {
            return static_cast<size_t>(t);
        }
    };

    using AnyListener = std::function<void(const PacketBase&, const PacketMetadata&)>;
    std::unordered_map<PacketType, std::vector<AnyListener>, EnumHash> listeners_;
};
