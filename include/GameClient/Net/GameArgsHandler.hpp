#pragma once

#include "Networking/Packets/PacketDistributor.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"
#include "Game/Worlds/ClientWorld.hpp"

#include "Renderer/Renderer.hpp"

class GameArgsHandler {
public:
    explicit GameArgsHandler(
        AssetDatabase& asset_db,
        ResourceManager& res,
        Renderer& renderer,
        PacketDistributor& distributor,
        ClientWorld& world    
    )
            : asset_db_(asset_db), res_(res), renderer_(renderer), world_(world) {
                registerListener(distributor);
            }

    bool loaded() const {return loaded_args_.has_value();}
private:
    void processPacket(const GameArgsPacket& pkt, const PacketMetadata& metadata) {
        const json& args = pkt.read_json();
        applyGameArgs(args);
    }

    void applyGameArgs(const json& game_args) {
        if (!validate(game_args)) {
            DEBUG_LOG("Canceled game args application");
            return;
        }

        if (loaded()) {
            if (game_args == *loaded_args_) {
                return;
            } else {
                DEBUG_LOG("args previously loaded but are different");
            }
        }
        // Load assets + upload
        for (const auto& asset_name_j : game_args.at("all_assets")) {
            const std::string asset_name = asset_name_j.get<std::string>();
            const auto meta = asset_db_.Get(asset_name);
            res_.loadAsset(meta.asset_path, asset_name);

            Asset* asset = res_.getAsset(asset_name);
            if (!asset) {
                DEBUG_LOG("asset was null after load: " << asset_name);
                continue;
            }
            DEBUG_LOG("Loaded asset, name: " << asset_name);
        }

        // Map
        const std::string map_name = game_args.at("map").get<std::string>();
        Asset* a = res_.getAsset(map_name);
        auto& mesh_id = a->mesh_id;

        Transform t;
        t.position = {0.0f,0.0f,17.5f};
        world_.add<ClientArchetypeId::Map>(t, mesh_id);


        DEBUG_LOG("assets loaded");

        loaded_args_.emplace(game_args);
    }

    const std::string& activeCharacter() const { return active_character_; }

    void registerListener(PacketDistributor& distributor) {
        distributor.on<GameArgsPacket>(
            PacketType::GAME_ARGS_PACKET, 
            [this](const GameArgsPacket& pkt, const PacketMetadata& metadata) {
                processPacket(pkt, metadata);
            }
        );
        AutoRegisterPacket<
            GameArgsPacket,
            PacketType::GAME_ARGS_PACKET
        >::register_pkt();
    }

    static inline bool validate(const json& j) noexcept {
        for (auto key : {"map", "all_assets"}) {
            if (!j.contains(key)) {
                DEBUG_LOG("Packet missing required \"" << key << "\"");
                return false;
            }
        }
        return true;
    }

    AssetDatabase& asset_db_;
    ResourceManager& res_;
    Renderer& renderer_;
    ClientWorld& world_;
    std::string active_character_;

    std::optional<json> loaded_args_ = std::nullopt;
    bool loaded_;
};