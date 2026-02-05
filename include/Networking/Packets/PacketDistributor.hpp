#pragma once

#include "Networking/Packets/PacketTypes.hpp"
#include "Networking/Packets/PacketBase.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <stdexcept>

class PacketDistributor {
public:
    PacketDistributor() = default;

    // Register a typed listener for a PacketType.
    // The listener will receive TPacket&, already cast.
    template <typename TPacket>
    void on(PacketType type, std::function<void(const TPacket&)> fn) {
        static_assert(std::is_base_of_v<PacketBase, TPacket>,
                      "TPacket must derive from PacketBase");

        listeners_[type].push_back(
            [type, f = std::move(fn)](const PacketBase& base) {
                // Safety: ensure runtime type matches what the listener expects.
                // If you want "crash fast" replace with throw/assert.
                const auto* pkt = dynamic_cast<const TPacket*>(&base);
                if (!pkt) {
                    // Mismatch means your factory or getType() is inconsistent.
                    // Choose one behavior:
                    // 1) silently ignore (current)
                    // 2) throw
                    // 3) assert/log
                    return;

                    // throw std::runtime_error("PacketDistributor: type mismatch for " + std::to_string((int)type));
                }
                f(*pkt);
            }
        );
    }

    // Dispatch packet based on packet.getType().
    // Returns true if at least one listener ran.
    bool dispatch(const PacketBase& packet) const {
        auto it = listeners_.find(packet.getType());
        if (it == listeners_.end()) return false;

        for (const auto& cb : it->second) cb(packet);
        return !it->second.empty();
    }

    bool dispatch(const std::unique_ptr<PacketBase>& packet) const {
        return packet ? dispatch(*packet) : false;
    }

    void clear() { listeners_.clear(); }
    void clear(PacketType type) { listeners_.erase(type); }

private:
    struct EnumHash {
        size_t operator()(PacketType t) const noexcept {
            return static_cast<size_t>(t);
        }
    };

    using AnyListener = std::function<void(const PacketBase&)>;
    std::unordered_map<PacketType, std::vector<AnyListener>, EnumHash> listeners_;
};
