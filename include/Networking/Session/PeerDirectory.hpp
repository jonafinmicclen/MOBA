#pragma once

#include "Common/Player/AccountID.hpp"
#include "Networking/Core/NetCommon.hpp"

#include <unordered_map>
#include <optional>

class PeerDirectory {
public:
    void addAccount(const AccountID& acc_id, const PeerID& peer_id) {
        /** 
         * The latest peer id will always be the only valid hence overwrite.
         * When the peer id is not present i.e. a disconnect happens
         * this is no longer up to date which results in packets dropped
         * by the networker.
         */
        if (auto old_peer_id = find(acc_id)) {
            account_by_peer_.erase(*old_peer_id);
        }
        if (auto old_acc_id = find(peer_id)) {
            peer_by_account_.erase(*old_acc_id);
        }

        peer_by_account_.insert_or_assign(acc_id, peer_id);
        account_by_peer_.insert_or_assign(peer_id, acc_id);
    }
    std::optional<PeerID> find(const AccountID acc_id) const {
        if (auto peer_id = peer_by_account_.find(acc_id); peer_id != peer_by_account_.end()) {
            return peer_id->second;
        }
        return std::nullopt;
    }
    std::optional<AccountID> find(const PeerID peer_id) const {
        if (auto acc_id = account_by_peer_.find(peer_id); acc_id != account_by_peer_.end()) {
            return acc_id->second;
        }
        return std::nullopt;
    }


private:
    std::unordered_map<AccountID, PeerID> peer_by_account_;
    std::unordered_map<PeerID, AccountID> account_by_peer_;
};