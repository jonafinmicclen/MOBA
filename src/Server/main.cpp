#include "Networking/Server/NetServer.hpp"
#include "Networking/Client/NetClient.hpp"

#include <thread>
#include <chrono>

int main() {
    NetServer server(2, ENET_HOST_ANY, 1234);

    // server loop in a separate thread
    std::thread server_thread([&server]() {
        while (true) {
            server.pollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            server.sendPackets();
        }
    });

    // give server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    NetClient client("localhost", 1234);

    // client loop
    while (true) {
        client.pollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    server_thread.join();
    return 0;
}
