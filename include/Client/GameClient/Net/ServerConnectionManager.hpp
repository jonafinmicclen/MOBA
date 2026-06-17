#pragma once

#include "GameArgsHandler.hpp"

#include "Game/Packets/Initialiser/ClientAuthenticationPacket.hpp"

#include "Core/Networking/Core/NetEventDistributor.hpp"

#include "Core/Adapter/NetAdapter.hpp"

/**
 * connects on disconnect
 * connects once at start
 * applies game args when packet recieved
 * 
 */

class ServerConnectionManager {
public:
    ServerConnectionManager(std::string path_to_auth_json, NetAdapter& network_adapter, NetEventDistributor& distributor, ConnectCmd connect_command) 
        : network_adapter_(network_adapter), connect_command_(connect_command) {
        auth_packet_.json_from_file(path_to_auth_json);

        distributor.on<NetConnectEvent>([this](const NetConnectEvent& e) {
            onConnection();
        });

        distributor.on<NetDisconnectEvent>([this](const NetDisconnectEvent& e) {
            onDisconnect();
        });
    }

    void connect() {
        network_adapter_.command(connect_command_);
    }

private:
    void onConnection() {
        DEBUG_LOG("Connected");

        connected_ = true;
        network_adapter_.sendPacket(&auth_packet_, Channel::RELIABLEGAMEPLAY, {});
    }

    void onDisconnect() {
        DEBUG_LOG("Disconnected");

        connected_ = false;
        connect();
    }
   
    NetAdapter& network_adapter_;
    ConnectCmd connect_command_;
    std::optional<GameArgsHandler> args_handler_;

    ClientAuthenticationPacket auth_packet_;
    bool connected_ =  false;

};