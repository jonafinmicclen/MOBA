#pragma once

#include "Debug/debug.hpp"

#include "Threading/ThreadSafeQueue.hpp"
#include "Common/Memory/HandleMap.hpp"

#include "Networking/NetConfig.hpp"
#include "Networking/Message/NetMessage.hpp"
#include "Networking/Core/NetCommon.hpp"
#include "Networking/Core/NetCommand.hpp"
#include "Networking/Core/NetEvent.hpp"

#include "Server/GameArgs.hpp"

#include <enet/enet.h>

#include <stdexcept>
#include <queue>
#include <vector>
#include <thread>
#include <optional>


using NetMessageQueue = ThreadSafeQueue<NetMessage>;
using CommandQueue = ThreadSafeQueue<NetCommand>;
using EventQueue = ThreadSafeQueue<NetEvent>;


class Networker {
public:
    explicit Networker (const NetConfig& cfg);
    ~Networker();

    void                        pushMessage(NetMessage&& msg);
    std::optional<NetMessage>   popMessage();

    void                        pushCommand(NetCommand&& cmd);
    std::optional<NetEvent>     popEvent();

    void start();
    void stop();

protected:
    virtual void onRecieveCompute(const ENetEvent& e) {}
    virtual void onConnectCompute(const ENetEvent& e) {}
    virtual void onDisconnectCompute(const ENetEvent& e) {}

private:
    void pollEvents();
    void sendQueues();
    void pollCommands();

    void onRecieve(const ENetEvent& e);
    void onConnect(const ENetEvent& e);
    void onDisconnect(const ENetEvent& e);

    void loop(std::stop_token st);

    ENetHost* networker_;
    ENetEvent event_;

    NetMessageQueue outgoing_queue_;
    NetMessageQueue incoming_queue_;

    CommandQueue command_queue_;
    EventQueue event_queue_;

    HandleMap<ENetPeer*, PeerID> peer_map_;

    std::jthread thread_;
    NetConfig config_;
};