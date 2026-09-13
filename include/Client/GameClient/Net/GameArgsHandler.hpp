#pragma once

#include <algorithm>

#include "Core/Networking/Packets/PacketDistributor.hpp"
#include "Game/Packets/Initialiser/GameArgsPacket.hpp"
#include "Client/GameClient/ClientWorld.hpp"

#include "Client/Renderer/Renderer.hpp"
#include "Client/GameClient/Camera/Camera.hpp"
#include "Client/GameClient/MapPlacement.hpp"

#include "Game/MapDef.hpp"
#include "Game/MapDefLoader.hpp"
#include "Game/MapDatabase.hpp"

class GameArgsHandler {
public:
    explicit GameArgsHandler(
        AssetDatabase& asset_db,
        ResourceManager& res,
        Renderer& renderer,
        Camera& camera,
        PacketDistributor& distributor,
        ClientWorld& world
    )
            : asset_db_(asset_db), res_(res), renderer_(renderer), camera_(camera), world_(world) {
                registerListener(distributor);
            }

    bool loaded() const {return loaded_args_.has_value();}

    const MapDef* getMapDef() const { return map_def_ ? &*map_def_ : nullptr; }
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
        t.position = {0.0f, 0.0f, kMapGroundHeight};
        t.scale = {2.0f, 2.0f, 1.f};
        world_.add<ClientArchetypeId::Map>(t, mesh_id);

        map_def_.emplace(MapDefLoader::load(MapDatabase::instance().Get(map_name)));

        // Derive the camera's pan bounds from the walkable grid's actual
        // world-space extent (min/max in case scale is ever negative),
        // instead of a hardcoded guess.
        {
            const WalkableMap& walkable_area = map_def_->walkable_area;
            Vec2 corner_a = map_def_->map_from_world.applyInverse(Vec2{Fixed(0.0f), Fixed(0.0f)});
            Vec2 corner_b = map_def_->map_from_world.applyInverse(Vec2{
                Fixed(static_cast<float>(walkable_area.getWidth())),
                Fixed(static_cast<float>(walkable_area.getHeight()))
            });

            Rect pan_bounds;
            pan_bounds.min_corner = WorldSpacePos::fromVec2(Vec2{
                std::min(corner_a.x, corner_b.x),
                std::min(corner_a.y, corner_b.y)
            });
            pan_bounds.max_corner = WorldSpacePos::fromVec2(Vec2{
                std::max(corner_a.x, corner_b.x),
                std::max(corner_a.y, corner_b.y)
            });
            camera_.setPanBounds(pan_bounds);
        }

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
    Camera& camera_;
    ClientWorld& world_;
    std::string active_character_;

    std::optional<json> loaded_args_ = std::nullopt;
    std::optional<MapDef> map_def_;
    bool loaded_;
};