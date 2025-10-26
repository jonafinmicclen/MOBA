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
    std::cout<<"WTat\n\n";
    NetServer server(2, ENET_HOST_ANY, 1234);

    // Server loop in a separate thread
    std::thread server_thread([&server]() {
        std::cout<<"Herllo from server thread";
        EventPacketConverter converter;

        // Original event
        MoveCharacterEvent event;
        event.id = CharacterID{12};
        event.pos = WorldSpacePos{10, 9};

        std::cout<<"Hello?"<<std::endl;

        auto packetP  = converter.eventToPacket(&event);
        packetP->event = &event;
        std::cout<<"Pre cerreal";

        packetP->serialize();

        std::cout<<"Pre cast";

        auto packetPtr = dynamic_cast<MoveCharacterPacket*>(packetP);

        std::cout<<event.id.id;


        //packetP->serialize();
        if(packetPtr->event->GetType() == EventType::MoveCharacter) std::cout<<"correctype  ";
        if (!packetPtr) std::cout<<"eventtopacket failed";
    


        while (true) {
            server.pollEvents();

            if (packetPtr) {
                server.broadcastPackets(packetP, 0);
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
            BaseEvent* event = converter.packetToEvent(packet.get());

            if (event && event->GetType() == EventType::MoveCharacter) {
                auto moveEvent = dynamic_cast<MoveCharacterEvent*>(event);

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
