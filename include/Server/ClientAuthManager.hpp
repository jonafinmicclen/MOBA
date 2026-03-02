#pragma once

#include "Server/AccountCharacterBiMap.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "Networking/Core/Networker.hpp"
#include "Common/Player/AccountID.hpp"
#include "GameClient/Packets/ClientAuthenticationPacket.hpp"
#include "Networking/Session/PeerDirectory.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"
#include "Server/GameArgs.hpp"
#include "Adapter/NetAdapter.hpp"

class ClientAuthManager {
public:
    ClientAuthManager(PacketDistributor& distributor, NetAdapter& networker, GameArgs& game_args) 
        : networker_(networker), game_args_(game_args) {
        initListener(distributor);
    }
    PeerDirectory& getMap() {return map_;}
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
        if (!acc_id || metadata.ids.size() != 0) {
            // Disconnect them
            return;
        }
        auto character = game_args_.player_account_map.characterFor(*acc_id);
        if (!character) {
            // Disconnect them
            return;
        }
        auto peer = metadata.ids[0];
        map_.addAccount(*acc_id, peer);

        GameArgsPacket packet(game_args_);
        auto& json_data = packet.get_json();
        json_data["active_character"] = *character;

        networker_.sendPacket(&packet, Channel::RELIABLEGAMEPLAY, {peer});
    }

    PeerDirectory map_;
    NetAdapter& networker_;
    GameArgs& game_args_;
};