#pragma once

#include <functional>
#include <optional>

#include "Assets/ResourceManager.hpp"

#include "Game/Worlds/ClientWorld.hpp"

#include "Networking/Packets/PacketDistributor.hpp"
#include "Game/Packets/EntityOwnershipPacket.hpp"
#include "Game/Packets/EntityStatePacket.hpp"
#include "Game/Packets/SpawnPacket.hpp"

#include "Common/Memory/BiMap.hpp"

struct ServerHandle {
    EntityHandle handle;

    bool operator==(const ServerHandle& other) const noexcept {
        return handle == other.handle;
    }
};

struct ClientHandle {
    EntityHandle handle;

    bool operator==(const ClientHandle& other) const noexcept {
        return handle == other.handle;
    }
};

namespace std {
    template <>
    struct hash<ClientHandle> {
        std::size_t operator()(const ClientHandle& h) const noexcept {
            return EntityHandle::Hash{}(h.handle);
        }
    };

    template <>
    struct hash<ServerHandle> {
        std::size_t operator()(const ServerHandle& h) const noexcept {
            return EntityHandle::Hash{}(h.handle);
        }
    };
}

class DuplicationSystem {
public:
    DuplicationSystem(ClientWorld& world, PacketDistributor& distributor) : world_(world) {
        registerHandlers(distributor);
    }


private:
    ClientWorld& world_;
    BiMap<ClientHandle, ServerHandle> client_to_server_handle_;
    std::optional<ServerHandle> owned_handle_;

    void entityOwnershipHandler(const EntityOwnershipPacket& pkt, const PacketMetadata& metadata) {
        owned_handle_->handle = pkt.getData().server_handle;
    }
    void entityStateHandler(const EntityStatePacket& pkt, const PacketMetadata& metadata) {
        // Update state in world
    }
    void spawnHandler(const SpawnPacket& pkt, const PacketMetadata& metadata) {
        MeshId mesh = ResourceManager::instance().getAsset(pkt.getData().entity)->mesh_id;
        // Later should use the name/hash to lookup archetype and other initialiser data in database or res manager can do it
        
        world_.add<ClientArchetypeId::Champion>(pkt.getData().position, mesh, pkt.getData().server_handle);
    }

    void registerHandlers(PacketDistributor& distributor) {
        // EntityOwnershipPacket
        distributor.on<EntityOwnershipPacket>(
            PacketType::EntityOwnershipPacket, 
            [this](const EntityOwnershipPacket& pkt, const PacketMetadata& metadata) {
                entityOwnershipHandler(pkt, metadata);
            }
        );
        AutoRegisterPacket<
            EntityOwnershipPacket,
            PacketType::EntityOwnershipPacket
        >::register_pkt();

        // EntityStatePacket
        distributor.on<EntityStatePacket>(
            PacketType::EntityStatePacket, 
            [this](const EntityStatePacket& pkt, const PacketMetadata& metadata) {
                entityStateHandler(pkt, metadata);
            }
        );
        AutoRegisterPacket<
            EntityStatePacket,
            PacketType::EntityStatePacket
        >::register_pkt();

        // SpawnPacket
        distributor.on<SpawnPacket>(
            PacketType::SpawnPacket, 
            [this](const SpawnPacket& pkt, const PacketMetadata& metadata) {
                spawnHandler(pkt, metadata);
            }
        );
        AutoRegisterPacket<
            SpawnPacket,
            PacketType::SpawnPacket
        >::register_pkt();
    }
};