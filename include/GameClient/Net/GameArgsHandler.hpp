#pragma once

#include "Networking/Packets/PacketDistributor.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"

#include "Renderer/Renderer.hpp"
#include "Game/Game.hpp"

class GameArgsHandler {
public:
    explicit GameArgsHandler(
        AssetDatabase& asset_db,
        ResourceManager& res,
        Renderer& renderer,
        Game& game,
        PacketDistributor& distributor)
            : asset_db_(asset_db), res_(res), renderer_(renderer), game_(game) {
                registerListener(distributor);
            }

    bool loaded() const {return loaded_args_.has_value();}
private:
    void processPacket(const GameArgsPacket& pkt, const PacketMetadata& metadata) {
        const json& args = pkt.read_json();
        applyGameArgs(args);
    }

    void applyGameArgs(const json& game_args) {
        validate(game_args);

        if (loaded()) {
            if (game_args == *loaded_args_) return;
        } else {
            DEBUG_LOG("args previously loaded but are different");
        }
        MeshID mesh_id{0};
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
            mesh_id = renderer_.uploadAssetMesh(asset);
            DEBUG_LOG("loaded+uploaded " << asset_name);
        }

        // Map
        const std::string map_name = game_args.at("map").get<std::string>();
        game_.setMap(map_name, mesh_id);

        // Active character (store somewhere later)
        const std::string active_char = game_args.at("active_character").get<std::string>();
        active_character_ = active_char;

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

    static void validate(const json& j) {
        for (auto key : {"map", "active_character", "all_assets"}) {
            if (!j.contains(key)) {
                DEBUG_LOG("missing required arg: " << key);
            }
        }
    }

    AssetDatabase& asset_db_;
    ResourceManager& res_;
    Renderer& renderer_;
    Game& game_;
    std::string active_character_;

    std::optional<json> loaded_args_;
    bool loaded_;
};