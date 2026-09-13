#pragma once

#include <functional>
#include <optional>

#include "Assets/ResourceManager.hpp"

#include "Client/GameClient/ClientWorld.hpp"
#include "Client/GameClient/Camera/Camera.hpp"

#include "Core/Networking/Packets/PacketDistributor.hpp"
#include "Game/Packets/Initialiser/EntityOwnershipPacket.hpp"
#include "Game/Packets/Gameplay/EntityStatePacket.hpp"
#include "Game/Packets/Gameplay/SpawnPacket.hpp"

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
    DuplicationSystem(ClientWorld& world, PacketDistributor& distributor, Camera& camera)
        : world_(world), camera_(camera) {
        registerHandlers(distributor);
    }


private:
    ClientWorld& world_;
    Camera& camera_;
    BiMap<ClientHandle, ServerHandle> client_to_server_handle_;
    std::optional<ServerHandle> owned_handle_;

    void entityOwnershipHandler(const EntityOwnershipPacket& pkt, const PacketMetadata& metadata) {
        owned_handle_ = ServerHandle{pkt.getData().server_handle};

        // Center the camera on the entity the moment we learn we own it -
        // by this point its SpawnPacket has already arrived (same reliable
        // channel, sent first by ClientAuthSystem::beginGame), so it should
        // already exist in the client world.
        ClientHandle* c_handle = client_to_server_handle_.findByB(*owned_handle_);
        if (c_handle == nullptr) {
            DEBUG_LOG("Owned entity not found in client world when centering camera");
            return;
        }
        Transform* t = world_.tryGet<Transform>(c_handle->handle);
        if (t == nullptr) {
            DEBUG_LOG("Owned entity has no Transform when centering camera");
            return;
        }
        camera_.setCameraPos2D(WorldSpacePos{t->position.x, t->position.y});
    }
    void entityStateHandler(const EntityStatePacket& pkt, const PacketMetadata& metadata) {
        auto& data = pkt.getData();
        ServerHandle s_handle {data.handle};
        ClientHandle* c_handle = client_to_server_handle_.findByB(s_handle);
        if (c_handle == nullptr) {
            DEBUG_LOG("Server to client handle not assigned when recieving state update");
            return;
        }
        *world_.tryGet<Transform>(c_handle->handle) = data.new_transform;
    }
    void spawnHandler(const SpawnPacket& pkt, const PacketMetadata& metadata) {
        MeshId mesh = ResourceManager::instance().getAsset(pkt.getData().entity)->mesh_id;
        // Later should use the name/hash to lookup archetype and other initialiser data in database or res manager can do it
        
        EntityHandle c_handle = world_.add<ClientArchetypeId::Champion>(pkt.getData().position, mesh, pkt.getData().server_handle);
        ClientHandle client_handle {c_handle};
        ServerHandle server_handle {pkt.getData().server_handle};
        client_to_server_handle_.insert(client_handle, server_handle);
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