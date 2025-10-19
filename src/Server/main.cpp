#include "Networking/Server/NetServer.hpp"
#include "Networking/Client/NetClient.hpp"

#include "Networking/Packets/TestPacket.hpp"

#include <thread>
#include <chrono>

int main() {
    NetServer server(2, ENET_HOST_ANY, 1234);

    

    // server loop in a separate thread
    std::thread server_thread([&server]() {
        auto packet = TestPacket("Packet of balls");
        while (true) {
            server.pollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            server.broadcastPackets(packet.serialize(), 0);
        }
    });

    // give server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    NetClient client("localhost", 1234);

    // client loop
    while (true) {
        client.pollEvents();

        if (PacketBase* recieved_packet = client.popQueue()) {
            if (TestPacket* test = dynamic_cast<TestPacket*>(recieved_packet)) {
                std::cout<<test->packet_string<<std::endl;
            } else {
                std::cout<<"tf packet\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    server_thread.join();
    return 0;
}
