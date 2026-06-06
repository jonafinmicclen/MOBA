#pragma once

#include "Game/ECS/Archetype.hpp"
#include "Game/ECS/Entity.hpp"
#include "Game/Transform.hpp"
#include "Game/Navigation/Path.hpp"

#include <deque>
#include <cstdint>
#include <utility>
#include <vector>
#include <cassert>
#include <limits>
#include <type_traits>
#include <stdexcept>

template<typename T, typename... Ts>
inline constexpr bool contains_type_v = (std::is_same_v<T, Ts> || ...);

template<typename ArchetypeT, typename... Wanted>
struct ArchetypeHasComponents;

template<typename... Components, typename... Wanted>
struct ArchetypeHasComponents<Archetype<Components...>, Wanted...> {
    static constexpr bool value =
        (contains_type_v<Wanted, Components...> && ...);
};

template<typename ArchetypeT, typename... Wanted>
inline constexpr bool archetype_has_components_v =
    ArchetypeHasComponents<ArchetypeT, Wanted...>::value;

enum class ArchetypeId : uint8_t {
    Champion,
    Minion,
    ProjectileFree,
    ProjectileHoming,
    Turret,
    Map,
    None
};

struct EntityLocation {
    Generation gen = 0;
    Row row = std::numeric_limits<Row>::max();
    ArchetypeId kind = ArchetypeId::None;
    bool alive = false;
};

struct EntityHandle {
    EntityID eid;
    Generation gen;
};

class World {
public:
    template<typename... Wanted, typename Func>
    void queryColumns(Func&& func) {
        queryColumnsForArchetype<Wanted...>(champions, func);
        queryColumnsForArchetype<Wanted...>(map, func);
    }

    template<ArchetypeId archetype, typename... Args>
    EntityHandle add(Args&&... components) {

        // Prepare entity ID
        EntityID eid;
        if (!free_eid_list_.empty()) {
            eid = free_eid_list_.front();
            free_eid_list_.pop_front();
        } else {
            eid = next_eid_++;
            entities.resize(next_eid_);
        }

        Row row = std::numeric_limits<Row>::max();
        if constexpr (archetype == ArchetypeId::Champion) {
            row = champions.add(eid, std::forward<Args>(components)...);
        }
        else if constexpr (archetype == ArchetypeId::Map) {
            row = map.add(eid, std::forward<Args>(components)...);
        }

        EntityLocation& e = entities[eid];
        e.alive = true;
        e.kind = archetype;
        e.row = row;

        EntityHandle r;
        r.eid = eid;
        r.gen = e.gen;
        
        return r;
    }
    
    void removeEntity(EntityHandle entity) {
        if (entity.eid >= entities.size()) {
            return;
        }
        auto& loc = entities[entity.eid];

        if (!loc.alive || loc.gen != entity.gen) {
            return;
        }

        RemovedRow removed{};

        switch (loc.kind) {
            case ArchetypeId::Champion:
                removed = champions.removeAt(loc.row);
                break;
            case ArchetypeId::Map:
                removed = map.removeAt(loc.row);
                break;
            default:
                throw std::runtime_error("Unsupported archetype in World::add");
        }

        loc.alive = false;
        loc.gen++;

        if (removed.moved) {
            auto& moved_loc = entities[removed.moved_entity_id];
            moved_loc.row = removed.new_location;
        }

        free_eid_list_.push_front(entity.eid);
    }

private:
    std::deque<EntityID> free_eid_list_;
    EntityID next_eid_ = 0;
    std::vector<EntityLocation> entities;

    Archetype<Transform, uint32_t, Path> champions;
    Archetype<Transform, uint32_t> map;

    template<typename... Wanted, typename ArchetypeT, typename Func>
    void queryColumnsForArchetype(ArchetypeT& archetype, Func& func) {
        if constexpr (archetype_has_components_v<ArchetypeT, Wanted...>) {
            func(archetype.template getColumn<Wanted>()...);
        }
    }
};