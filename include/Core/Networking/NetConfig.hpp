#pragma once

#include <cstdint>

struct NetConfig {
    // Always one for Client, usually 10 for Server
    size_t max_connections;
    size_t num_channels;
    const char* address;
    uint16_t port;

    NetConfig() = delete;
    NetConfig(size_t max_conn, size_t num_chan, const char* addr, uint16_t prt) :
        max_connections(max_conn), 
        num_channels(num_chan), 
        address(addr), 
        port(prt) {}
};