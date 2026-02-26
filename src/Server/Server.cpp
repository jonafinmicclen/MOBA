#include "Server/Server.hpp"

void Server::initialise() {
    DEBUG_LOG("INITIALISATION PHAZE");
    netServer = std::make_unique<NetServer>(10);    // Max clients 10
    game = std::make_unique<Game>();
    ResourceManager::instance();
    packetDistributor = std::make_unique<PacketDistributor>();

    AutoRegisterPacket<
        ClientAuthenticationPacket,
        PacketType::CLIENT_AUTH_PACKET
    >::register_pkt();

    DEBUG_LOG("Initialising packet listeners");
    initialise_packet_listeners();
    DEBUG_LOG("Parsing game args");
    parseGameArgsJSON();
}


void Server::parseGameArgsJSON() {
    std::ifstream f("RuntimeData/game_args.json");
    json game_args = json::parse(f);

    // Load map
    game->setMap(game_args["map"]);
    ResourceManager::instance().loadAsset(game_args["map"]);

    for (auto& [hash, character] : game_args["players"].items()) {
        user_id playerHash = hash;
        std::string characterName = character.get<std::string>();
        character_by_user_id[playerHash] = characterName;
        ResourceManager::instance().loadAsset(characterName);
    }
}

void Server::loadAssetByName(std::string name) {
    ResourceManager::instance().loadAsset(name);
}


void Server::beginSimulation() {
    initialise();
    running = true;
    DEBUG_LOG("Starting -- MAIN PHAZE");
    while (running) {
        netServer->pollEvents();
        distributePackets();
    }
}

void Server::distributePackets() {
    // Distribute all queued (recieved) packets
    while (std::unique_ptr<Message> message = netServer->popPacket(PacketFlag::GAMEPLAY)) {

        if (!message->header.id.has_value()) {
            DEBUG_LOG("Packet was missing an id. The program will die.");
        }

        if (!message->packet) {
            DEBUG_LOG("Packet was nullptr");
        }

        DEBUG_LOG("Distributing packet; Type: " << (int)message->packet->getType());

        bool status = packetDistributor->dispatch(message->packet, *message->header.id);
        DEBUG_LOG("dispatch status: " << (status ? "SUCCESS" : "FAIL"));
        // message now out of scope forever
        // probably fine just a reminder
    }
}

void Server::initialise_packet_listeners() {

    // Client auth distributor
    packetDistributor->on<ClientAuthenticationPacket>(
        PacketType::CLIENT_AUTH_PACKET, 
        [this](const ClientAuthenticationPacket& pkt, const uint8_t id)  {
            DEBUG_LOG("distributor met");
            authenticate_client(pkt, id);
        }
    );

}

void Server::authenticate_client(const ClientAuthenticationPacket& pkt, const uint8_t client_id) {
    // Sets up clients hash and validates it
    auto user_id = pkt.get_hash();
    if (!user_id) {
        DEBUG_LOG("Client auth packet did not contain a hash");
        // I dont think this is neccessary lel
        user_id_by_client_id[client_id] = "";
        netServer->disconnectByID(client_id);
        return; 
    }
    DEBUG_LOG("Client: " << (int)client_id << ", with UID: " << *user_id << " connected");
    user_id_by_client_id[client_id] = *user_id;

    initialise_client(*user_id, client_id);
}

void Server::initialise_client(const std::string user_id, const uint8_t client_id) {
    // Tell client what to load and which character its playing etc.

    std::string clients_active_character = character_by_user_id[user_id];
    std::string active_map_name = game->getMap().getName();
    std::vector<std::string> all_entities {};

    // Populate all entities vec
    auto states = game->getStates();
    all_entities.reserve(states.size());
    for (auto& state : states) {
        all_entities.push_back(state->name);
    }

    // Create JSON
    json args;
    args["active_character"] = clients_active_character;
    args["map"] = active_map_name;

    #ifdef DEBUG
    args["all_assets"] = std::vector<std::string> {"Naren", "Summoners Rift"};
    #else
    args["all_assets"] = all_entities;
    #endif

    GameArgsPacket pkt;
    pkt.set_json(args);

    netServer->sendPackets(static_cast<PacketBase*>(&pkt), client_id, 0);
}