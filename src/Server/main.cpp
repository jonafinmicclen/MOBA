#include "Networking/Server/NetServer.hpp"
#include "Networking/Client/NetClient.hpp"

#include "Networking/Packets/Types/MoveCharacterPacket.hpp"
#include "Game/Events/MoveCharacterEvent.hpp"
#include "Common/Coordinates/WorldSpacePos.hpp"
#include "Game/NetworkAdapters/EventPacketConverter.hpp"

#include <thread>
#include <chrono>
#include <iostream>

int main() {
    NetServer server(2, ENET_HOST_ANY, 1234);

    // Server loop in a separate thread
    std::thread server_thread([&server]() {
        EventPacketConverter converter;

        // Original event
        MoveCharacterEvent event;
        event.id = CharacterID{12};
        event.pos = WorldSpacePos{10, 9};

        while (true) {
            server.pollEvents();

            // Make a fresh unique_ptr for every broadcast
            std::unique_ptr<BaseEvent> eventPtr = std::make_unique<MoveCharacterEvent>(event);
            auto packet = converter.eventToPacket(std::move(eventPtr));

            if (packet) {
                server.broadcastPackets(packet, 0);
            } else std::cout<<"Packet was empty";

            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
    });

    // Give server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Client
    NetClient client("localhost", 1234);
    EventPacketConverter converter;

    // Client loop
    while (true) {
        client.pollEvents();

        if (auto packet = client.popQueue()) {
            std::unique_ptr<BaseEvent> event = converter.packetToEvent(std::move(packet));

            if (event && event->GetType() == EventType::MoveCharacter) {
                auto moveEvent = dynamic_cast<MoveCharacterEvent*>(event.get());

                if (moveEvent) {
                    std::cout << "ID: " << moveEvent->id.id
                              << ", X=" << moveEvent->pos.x
                              << ", Y=" << moveEvent->pos.y
                              << std::endl;
                }
            }
        } else std::cout<<"Queue was empty";

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    server_thread.join();
    return 0;
}
