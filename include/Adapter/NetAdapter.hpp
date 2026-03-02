#pragma once

#include "Common/Player/AccountID.hpp"

#include "Networking/Core/NetCommon.hpp"
#include "Networking/Core/Networker.hpp"
#include "Networking/Packets/PacketBase.hpp"

#include <enet/enet.h>

#include <vector>


enum class PacketFlag : enet_uint32 {
    RELIABLE = ENetPacketFlag::ENET_PACKET_FLAG_RELIABLE,
    UNSEQUENCED = ENetPacketFlag::ENET_PACKET_FLAG_UNSEQUENCED,
    SEQUENCED_UNRELIABLE = 0
};

enum class Channel : ChannelIDX {
    RELIABLEGAMEPLAY = 0,   
    STATEUPDATES     = 1,
    RELIABLECOMMANDS = 2,
};

using TargetPeers = std::vector<PeerID>;

class NetAdapter {
public:
    NetAdapter(Networker& networker) : networker_(networker) {}

    void sendPacket(const PacketBase *const packet, const Channel channel, const TargetPeers targets) {
        
        NetMessage message(
                static_cast<ChannelIDX>(channel),
                flagsFor(channel),
                packet->serialize(),
                targets
        );

        networker_.pushMessage(std::move(message));
    }

private:
    Networker& networker_;

    static constexpr enet_uint32 flagsFor(Channel c) {
        switch (c) {
            case Channel::RELIABLEGAMEPLAY: return ENET_PACKET_FLAG_RELIABLE;
            case Channel::STATEUPDATES:     return 0; // unreliable + sequenced
            case Channel::RELIABLECOMMANDS: return ENET_PACKET_FLAG_RELIABLE;
        }
    }
};