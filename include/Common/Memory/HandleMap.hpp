#pragma once
#include <unordered_map>
#include <cstdint>
#include <optional>

template <typename Ptr, typename ID = std::uint32_t>
class HandleMap {
public:
    ID insert(Ptr ptr) {
        const ID id = next_id_++;
        id_by_ptr_[ptr] = id;
        ptr_by_id_[id] = ptr;
        return id;
    }

    bool erase(Ptr ptr) {
        auto it = id_by_ptr_.find(ptr);
        if (it == id_by_ptr_.end()) return false;
        ID id = it->second;
        id_by_ptr_.erase(it);
        ptr_by_id_.erase(id);
        return true;
    }

    std::optional<ID> find(Ptr ptr) const {
        auto it = id_by_ptr_.find(ptr);
        if (it == id_by_ptr_.end()) return std::nullopt;
        return it->second;
    }

    std::optional<Ptr> find(ID id) const {
        auto it = ptr_by_id_.find(id);
        if (it == ptr_by_id_.end()) return std::nullopt;
        return it->second;
    }

    ID at(Ptr ptr) const { return id_by_ptr_.at(ptr); }
    Ptr at(ID id) const { return ptr_by_id_.at(id); }

private:
    ID next_id_ = 0;
    std::unordered_map<ID, Ptr> ptr_by_id_;
    std::unordered_map<Ptr, ID> id_by_ptr_;
};