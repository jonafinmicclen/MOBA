#pragma once

#include <vector>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>
#include <optional>

#include "Networking/Core/Networker.hpp"
#include "Networking/Core/NetEvent.hpp" // <-- wherever NetEvent/NetConnectEvent/... live

class NetEventDistributor {
public:
    explicit NetEventDistributor(Networker& networker)
        : networker_(networker) {}

    // Pump all queued events and dispatch to handlers
    void pump() {
        while (auto evOpt = networker_.popEvent()) {
            const NetEvent& ev = *evOpt;

            // Global handlers (see everything)
            for (auto& h : anyHandlers_) {
                h(ev);
            }

            // Type-specific handlers
            std::visit([this](auto const& payload) {
                using T = std::decay_t<decltype(payload)>;
                dispatch<T>(payload);
            }, ev);
        }
    }

    // Register a handler that receives the whole variant
    // Example: onAny([](const NetEvent& e){ ... })
    void onAny(std::function<void(const NetEvent&)> handler) {
        anyHandlers_.push_back(std::move(handler));
    }

    // Register handler for a specific payload type
    // Example: on<NetConnectEvent>([](const NetConnectEvent& e){ ... })
    template <class T, class F>
    void on(F&& fn) {
        static_assert(is_in_variant<T, NetEvent>::value,
                      "T must be one of the NetEvent variant alternatives");

        auto wrapper = [f = std::forward<F>(fn)](const T& e) mutable {
            std::invoke(f, e);
        };

        handlers<T>().push_back(std::move(wrapper));
    }

private:
    Networker& networker_;

    // --- storage for handlers ---
    std::vector<std::function<void(const NetEvent&)>> anyHandlers_;
    std::vector<std::function<void(const NetConnectEvent&)>> connectHandlers_;
    std::vector<std::function<void(const NetDisconnectEvent&)>> disconnectHandlers_;

    // --- dispatch helper ---
    template <class T>
    void dispatch(const T& payload) {
        for (auto& h : handlers<T>()) {
            h(payload);
        }
    }

    // --- select the right handler list for T ---
    template <class T>
    std::vector<std::function<void(const T&)>>& handlers() {
        if constexpr (std::is_same_v<T, NetConnectEvent>) {
            return connectHandlers_;
        } else if constexpr (std::is_same_v<T, NetDisconnectEvent>) {
            return disconnectHandlers_;
        } else {
            static_assert(always_false<T>::value, "No handler list for this event type");
        }
    }

    template <class>
    struct always_false : std::false_type {};

    // --- compile-time check: T is an alternative in variant V ---
    template <class T, class V>
    struct is_in_variant;

    template <class T, class... Alts>
    struct is_in_variant<T, std::variant<Alts...>>
        : std::bool_constant<(std::is_same_v<T, Alts> || ...)> {};
};