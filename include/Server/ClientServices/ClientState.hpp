#pragma once

#include "EntityComponentSystem/Worlds/ServerWorld.hpp"
#include "Authentication/Auth.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Networking/Session/PeerDirectory.hpp"

using AccountEntityMap = BiMap<AccountHash, EntityHandle>;

struct ClientState {
    AccountEntityMap account_entity_map;
    PeerAccountMap peer_id_account_hash_map;
};
