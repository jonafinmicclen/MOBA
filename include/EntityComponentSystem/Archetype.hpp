#pragma once

#include "EntityComponentSystem/Entity.hpp"
#include "EntityComponentSystem/ECSConfig.hpp"

#include <span>
#include <cstdint>
#include <array>
#include <tuple>
#include <utility>
#include <type_traits>
#include <cassert>

/**
 * Archetype stores an individual archetype
 * 
 * Stores separate vectors of things
 */

struct RemovedRow {
    bool moved = false;
    EntityID moved_entity_id;
    Row new_location;
};


template <typename... Components>
class Archetype {
public:

    template <typename T>
    std::span<T> getColumn() {
        auto& arr = std::get<std::array<T, ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE>>(columns_);
        return std::span<T>(arr.data(), size());
    }

    template <typename T>
    std::span<const T> getColumn() const {
        const auto& arr = std::get<std::array<T, ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE>>(columns_);
        return std::span<const T>(arr.data(), size());
    }

    std::span<const EntityID> getEntityIds() const {
        return std::span<const EntityID>(entities_.data(), size());
    }

    template<typename... Args>
    [[nodiscard]] Row add(EntityID eid, Args&&... components) {
        /**
         * Return index to use for entity tracking
         */
        static_assert(sizeof...(Args) == sizeof...(Components));
        static_assert((std::is_same_v<std::decay_t<Args>, Components> && ...));
        
        uint16_t next = getNextFree();

        ((std::get<std::array<Components, ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE>>(columns_)[next] =
        std::forward<Args>(components)), ...);
        entities_[next] = eid;

        return next;
    }

    RemovedRow removeAt(size_t index) {
        assert(index < size() && "Attempted to remove out of range index from archetype");

        RemovedRow result{};

        size_t last = next_free_ - 1;
        next_free_--;

        // Swap and pop
        if (index != last) {
            ((std::get<std::array<Components, ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE>>(columns_)[index] = 
                std::get<std::array<Components, ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE>>(columns_)[last]), ...);
            entities_[index] = entities_[last];

            result.moved = true;
            result.moved_entity_id = entities_[index];
            result.new_location = static_cast<Row>(index);
        }

        return result;
    }

    size_t size() const {
        // Next free should always be equal to the size
        return next_free_;
    }

    template<typename T>
    T& get(Row row) {
        return getColumn<T>()[row];
    }

    template<typename T>
    const T& get(Row row) const {
        return getColumn<T>()[row];
    }

private:
    size_t next_free_ = 0;
    std::tuple<std::array<Components, ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE>...> columns_;
    std::array<EntityID, ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE> entities_;

    uint16_t getNextFree() {
        /**
         * Gets next free index
         */
        assert(next_free_ < ECS_CONFIG::MAX_ENTITY_PER_ARCHETYPE && "Tried to allocate entity to full archetype.");
        return static_cast<uint16_t>(next_free_++);
    }
};


