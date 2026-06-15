#pragma once

#include "Server/ClientServices/AccountCharacterBiMap.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "Networking/Core/Networker.hpp"
#include "Common/Player/AccountID.hpp"
#include "GameClient/Packets/ClientAuthenticationPacket.hpp"
#include "Networking/Session/PeerDirectory.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"
#include "Server/GameArgs.hpp"
#include "Adapter/NetAdapter.hpp"

#include "EntityComponentSystem/Worlds/ServerWorld.hpp"
#include "Adapter/NetAdapter.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Game/Placeholder/PlaceholderMapDef.hpp"
#include "Game/Packets/SpawnPacket.hpp"
#include "Game/Packets/EntityOwnershipPacket.hpp"
#include "Server/ClientServices/ClientState.hpp"

#include <cstdint>
#include "Debug/debug.hpp"

   // net_adapter, account_entity_map_, game_map-, world_

class ClientAuthSystem {
    /**
     * Listens for clients authentication packet
     * Once authenticated maps peer -> account
     * Tells client what data to load
     */
public:
    ClientAuthSystem(
        PacketDistributor& distributor, 
        NetAdapter& networker, 
        GameArgs& game_args,
        BiMap<AccountHash, EntityHandle>& acc_entity_map,
        MapDef& game_map,
        ServerWorld& world,
        PeerAccountMap& map
    ) 
        : 
        networker_(networker), 
        game_args_(game_args),
        account_entity_map_(acc_entity_map),
        game_map_(game_map),
        world_(world),
        map_(map)
        {
        DEBUG_LOG("Game will start when " << (game_args_.players.size()) << " players are connected.");
        initListener(distributor);
    }
    PeerAccountMap& getMap() {return map_;}
private:

    void initListener(PacketDistributor& distributor) {
        distributor.on<ClientAuthenticationPacket>(
            PacketType::CLIENT_AUTH_PACKET, 
            [this](const ClientAuthenticationPacket& pkt, const PacketMetadata& metadata)  {
                authenticate(pkt, metadata);
            }
        );
        AutoRegisterPacket<
            ClientAuthenticationPacket,
            PacketType::CLIENT_AUTH_PACKET
        >::register_pkt();
    }

    void authenticate(const ClientAuthenticationPacket& pkt, const PacketMetadata& metadata) {
        auto acc_id = pkt.getHash();
        if (!acc_id || metadata.ids.size() != 1) {
            DEBUG_LOG("disconnected as malformed auth");
            networker_.command(DisconnectCmd(metadata.ids[0]));
            // Disconnect them
            return;
        }
        auto character = game_args_.player_account_map_onedir.find(*acc_id);
        
        if (character == game_args_.player_account_map_onedir.end()) {
            // Disconnect them
            DEBUG_LOG("Peer " << (int)metadata.ids[0] << " disconnected as no character assigned for account hash " << *acc_id << " in map");
            DEBUG_LOG("Assigned hashes are: ");
            for (auto& c : game_args_.players) {
                DEBUG_LOG(c.account);
            }
            networker_.command(DisconnectCmd(metadata.ids[0]));
            return;
        }

        auto peer = metadata.ids[0];
        map_.addAccount(*acc_id, peer);

        GameArgsPacket packet = GameArgsPacket::fromArgs(game_args_);

        networker_.sendPacket(&packet, Channel::RELIABLEGAMEPLAY, {peer});

        DEBUG_LOG("client authenticated");
        connected_players_++;
        if (shouldBeginGame()) {
            beginGame();
        }
    }

    bool shouldBeginGame() {
        // Should never be more than but oh well
        // This also does not take into account disconnects...
        return connected_players_ >= game_args_.players.size();
    }

    void beginGame() {
        // This should just be emitting an event
        for (auto& player : game_args_.players) {
            // Create the player entity
            SpawnPoint player_spawn = game_map_.spawn_points[player.player_idx];
            Transform spawn_transform;
            spawn_transform.position.x = player_spawn.point.x;
            spawn_transform.position.y = player_spawn.point.y;
            Path p;
            EntityHandle handle = world_.add<ServerArchetypeId::Champion>(
                spawn_transform, p, player.team, player_spawn
            );
            // Emit spawn packet for clients
            SpawnCommand c;
            c.entity = player.character;
            c.position = spawn_transform;
            c.server_handle = handle;
            SpawnPacket pkt;
            pkt.setData(c);

         

            networker_.sendPacket(&pkt, Channel::RELIABLECOMMANDS, {});

            // Register ownership to client
            EntityOwnershipPacket ownership_pkt;
            EntityOwnershipUpdate ownership_updt {handle};
            ownership_pkt.setData(ownership_updt);
            DEBUG_LOG("Mapping " << player.account << " to " << handle.eid << handle.gen);
            account_entity_map_.insert(player.account, handle);
            auto target_peer = map_.find(player.account);
            assert(target_peer && "Peer not assigned when sending ownership");
            networker_.sendPacket(&ownership_pkt, Channel::RELIABLECOMMANDS, {*target_peer});
        }
    }

    NetAdapter& networker_;
    GameArgs& game_args_;
    BiMap<AccountHash, EntityHandle>& account_entity_map_;
    MapDef& game_map_;
    ServerWorld& world_;
    PeerAccountMap& map_;
    uint8_t connected_players_ {0};
};