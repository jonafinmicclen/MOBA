#pragma once

#include <unordered_map>

template<typename A, typename B>
class BiMap {
public:
    bool insert(A a, B b) {
        if (a_to_b_.contains(a) || b_to_a_.contains(b)) {
            return false;
        }

        a_to_b_.emplace(a, b);
        b_to_a_.emplace(b, a);
        return true;
    }

    void eraseByA(const A& a) {
        auto it = a_to_b_.find(a);
        if (it == a_to_b_.end()) {
            return;
        }

        B b = it->second;
        a_to_b_.erase(it);
        b_to_a_.erase(b);
    }

    void eraseByB(const B& b) {
        auto it = b_to_a_.find(b);
        if (it == b_to_a_.end()) {
            return;
        }

        A a = it->second;
        b_to_a_.erase(it);
        a_to_b_.erase(a);
    }

    B* findByA(const A& a) {
        auto it = a_to_b_.find(a);
        if (it == a_to_b_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    A* findByB(const B& b) {
        auto it = b_to_a_.find(b);
        if (it == b_to_a_.end()) {
            return nullptr;
        }
        return &it->second;
    }

private:
    std::unordered_map<A, B> a_to_b_;
    std::unordered_map<B, A> b_to_a_;
};