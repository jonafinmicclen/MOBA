#pragma once

#include "Common/Debug/debug.hpp"

#include <cstdint>
#include <optional>

using PeerID = uint8_t;
using AccountID = uint64_t;

class PeerHandle {
public:
    Peer(P peer, PeerID id) : 
    peer_(peer), id_(id) {}

    P getPeer() const noexcept {
        return peer_;
    }

    PeerID getID() const noexcept {
        return id_;
    }

    void setAccountID(AccountID acc_id) noexcept {
        if (!account_id_) {
            account_id_ = acc_id;
            return true;
        } else {
            DEBUG_LOG("account id already set");
            return false;
        }
    }


private:
    const P peer_;
    const PeerID id_;
    std::optional<AccountID> account_id_ = std::nullopt;
};