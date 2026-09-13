#pragma once

#ifdef DEBUG

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Console debug stat overlay. Register/update a named value from anywhere
// with DEBUG_STAT(name, value), then call DEBUG_STAT_FLUSH() once per
// frame/tick to redraw the current set of values in place (like `top`),
// in the order each name was first registered.
//
// Entirely compiled out in Release builds - DEBUG is only defined for Debug
// builds (see top-level CMakeLists.txt), so call sites cost nothing there.
class DebugOverlay {
public:
    static DebugOverlay& instance() {
        static DebugOverlay overlay;
        return overlay;
    }

    template<typename T>
    void set(const std::string& name, const T& value) {
        std::ostringstream oss;
        oss << value;

        auto it = index_.find(name);
        if (it == index_.end()) {
            index_[name] = entries_.size();
            entries_.push_back({name, oss.str()});
        } else {
            entries_[it->second].second = oss.str();
        }
    }

    void flush() {
        // Move back up over the block printed last flush and clear each
        // line before reprinting, so it redraws in place instead of
        // scrolling the terminal.
        for (size_t i = 0; i < last_line_count_; ++i) {
            std::cout << "\x1b[1A\x1b[2K";
        }

        for (auto& [name, value] : entries_) {
            std::cout << name << ": " << value << "\n";
        }

        last_line_count_ = entries_.size();
    }

private:
    DebugOverlay() = default;

    std::vector<std::pair<std::string, std::string>> entries_;
    std::unordered_map<std::string, size_t> index_;
    size_t last_line_count_ = 0;
};

#define DEBUG_STAT(name, value) DebugOverlay::instance().set(name, value)
#define DEBUG_STAT_FLUSH() DebugOverlay::instance().flush()

#else

#define DEBUG_STAT(name, value) do {} while(0)
#define DEBUG_STAT_FLUSH() do {} while(0)

#endif
