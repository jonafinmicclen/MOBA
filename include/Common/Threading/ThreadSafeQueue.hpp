#pragma once

#include <queue>
#include <mutex>
#include <optional>

// Single producer-consumer queue

template <typename T>
class ThreadSafeQueue {

public:
void pushQueue(T item) {
    std::lock_guard<std::mutex> guard(mutex_);
    queue_.push(item);
}
std::optional<T> popQueue() {
    std::lock_guard<std::mutex> guard(mutex_);
    if (queue_.empty()) {
        return std::nullopt;
    }
    T item = std::move(queue_.front());
    queue_.pop();
    return item;
}

private:
std::queue<T> queue_;
std::mutex mutex_;
};