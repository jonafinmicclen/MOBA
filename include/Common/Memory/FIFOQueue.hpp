#pragma once

#include <deque>
#include <optional>

template<typename T>
class FIFOQueue {
public:
    std::optional<T> pop() {
        if (empty()) return std::nullopt;
        T v = std::move(q_.front());
        q_.pop_front();
        return v;
    }
    void push(T v) {
        q_.push_back(std::move(v));
    }
    bool empty() const {
        return q_.empty();
    }
    std::size_t size() const {
        return q_.size();
    }
private:
    std::deque<T> q_;
};