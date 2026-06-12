#pragma once

#include "Game/ECS/Archetype.hpp"
#include "Game/ECS/Entity.hpp"

#include <deque>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <tuple>
#include <functional>

// ------------------------------------------------------------
// Component checking helpers
// ------------------------------------------------------------

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


// ------------------------------------------------------------
// Archetype spec
// ------------------------------------------------------------
//
// This lets each World traits define its own enum class ArchetypeId.
//
// Example:
//
// ArchetypeSpec<
//     ServerArchetypeId::Champion,
//     Archetype<Transform, Team>
// >
//

template<auto Id, typename ArchetypeT>
struct ArchetypeSpec {
    static constexpr auto id = Id;
    using id_type = decltype(Id);
    using type = ArchetypeT;
};


// ------------------------------------------------------------
// Entity location
// ------------------------------------------------------------

template<typename ArchetypeIdT>
struct EntityLocation {
    Generation gen = 0;
    Row row = std::numeric_limits<Row>::max();
    ArchetypeIdT kind = ArchetypeIdT::None;
    bool alive = false;
};


// ------------------------------------------------------------
// Entity handle
// ------------------------------------------------------------

struct EntityHandle {
    EntityID eid;
    Generation gen;

    static_assert(
        sizeof(EntityID) == 2 && sizeof(Generation) == 2,
        "EntityHandle hashing assumes EntityID and Generation are both 16-bit"
    );

    bool operator==(const EntityHandle& other) const noexcept {
        return eid == other.eid && gen == other.gen;
    }

    struct Hash {
        std::size_t operator()(const EntityHandle& h) const noexcept {
            uint32_t packed =
                (static_cast<uint32_t>(h.gen) << 16) |
                 static_cast<uint32_t>(h.eid);

            return std::hash<uint32_t>{}(packed);
        }
    };
};

namespace std {
    template <>
    struct hash<EntityHandle> {
        std::size_t operator()(const EntityHandle& h) const noexcept {
            return EntityHandle::Hash{}(h);
        }
    };
}

// ------------------------------------------------------------
// World
// ------------------------------------------------------------

template<typename Traits>
class World {
private:
    using ArchetypeId = typename Traits::ArchetypeId;
    using Location = EntityLocation<ArchetypeId>;

    using ArchetypeSpecs = typename Traits::Archetypes;

    template<typename SpecsTuple>
    struct ArchetypeTupleFromSpecs;

    template<typename... Specs>
    struct ArchetypeTupleFromSpecs<std::tuple<Specs...>> {
        using type = std::tuple<typename Specs::type...>;
    };

    using ArchetypeStorage =
        typename ArchetypeTupleFromSpecs<ArchetypeSpecs>::type;

public:
    template<ArchetypeId archetype, typename... Args>
    EntityHandle add(Args&&... components) {
        EntityID eid;

        if (!free_eid_list_.empty()) {
            eid = free_eid_list_.front();
            free_eid_list_.pop_front();
        } else {
            eid = next_eid_++;
            entities.resize(next_eid_);
        }

        auto& storage = getArchetype<archetype>();

        Row row = storage.add(eid, std::forward<Args>(components)...);

        Location& e = entities[eid];
        e.alive = true;
        e.kind = archetype;
        e.row = row;

        return EntityHandle{
            .eid = eid,
            .gen = e.gen
        };
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
        bool found = false;

        removeFromMatchingArchetype<0>(loc.kind, loc.row, removed, found);

        if (!found) {
            throw std::runtime_error("Unsupported archetype in World::removeEntity");
        }

        loc.alive = false;
        loc.gen++;

        if (removed.moved) {
            auto& moved_loc = entities[removed.moved_entity_id];
            moved_loc.row = removed.new_location;
        }

        free_eid_list_.push_front(entity.eid);
    }

    template<typename T>
    T* tryGet(EntityHandle entity) {
        if (entity.eid >= entities.size()) {
            DEBUG_LOG("Entity ID " << entity.eid << " is more than or equal to size entities " << entities.size());
            return nullptr;
        }

        Location& loc = entities[entity.eid];

        if (!loc.alive || loc.gen != entity.gen) {
            DEBUG_LOG("Not alive or something");
            return nullptr;
        }

        return tryGetFromMatchingArchetype<T, 0>(loc.kind, loc.row);
    }

    template<typename... Wanted, typename Func>
    void queryColumns(Func&& func) {
        queryColumnsImpl<Wanted...>(
            std::forward<Func>(func),
            std::make_index_sequence<std::tuple_size_v<ArchetypeStorage>>{}
        );
    }

private:
    std::deque<EntityID> free_eid_list_;
    EntityID next_eid_ = 0;
    std::vector<Location> entities;

    ArchetypeStorage archetypes_;

private:
    template<ArchetypeId Id, std::size_t I = 0>
    auto& getArchetype() {
        if constexpr (I >= std::tuple_size_v<ArchetypeSpecs>) {
            static_assert(
                I < std::tuple_size_v<ArchetypeSpecs>,
                "ArchetypeId not found in World traits"
            );
        } else {
            using Spec = std::tuple_element_t<I, ArchetypeSpecs>;

            if constexpr (Spec::id == Id) {
                return std::get<I>(archetypes_);
            } else {
                return getArchetype<Id, I + 1>();
            }
        }
    }

    template<std::size_t I>
    void removeFromMatchingArchetype(
        ArchetypeId kind,
        Row row,
        RemovedRow& removed,
        bool& found
    ) {
        if constexpr (I < std::tuple_size_v<ArchetypeSpecs>) {
            using Spec = std::tuple_element_t<I, ArchetypeSpecs>;

            if (!found && Spec::id == kind) {
                removed = std::get<I>(archetypes_).removeAt(row);
                found = true;
                return;
            }

            removeFromMatchingArchetype<I + 1>(kind, row, removed, found);
        }
    }

    template<typename T, std::size_t I>
    T* tryGetFromMatchingArchetype(ArchetypeId kind, Row row) {
        if constexpr (I >= std::tuple_size_v<ArchetypeSpecs>) {
            return nullptr;
        } else {
            using Spec = std::tuple_element_t<I, ArchetypeSpecs>;
            using ArchetypeT = typename Spec::type;

            if (Spec::id == kind) {
                if constexpr (archetype_has_components_v<ArchetypeT, T>) {
                    return &std::get<I>(archetypes_).template get<T>(row);
                } else {
                    return nullptr;
                }
            }

            return tryGetFromMatchingArchetype<T, I + 1>(kind, row);
        }
    }

    template<typename... Wanted, typename Func, std::size_t... Is>
    void queryColumnsImpl(Func&& func, std::index_sequence<Is...>) {
        (
            queryColumnsForOneArchetype<Wanted...>(
                std::get<Is>(archetypes_),
                func
            ),
            ...
        );
    }

    template<typename... Wanted, typename ArchetypeT, typename Func>
    void queryColumnsForOneArchetype(ArchetypeT& archetype, Func& func) {
        if constexpr (archetype_has_components_v<ArchetypeT, Wanted...>) {
            func(archetype.template getColumn<Wanted>()...);
        }
    }
};