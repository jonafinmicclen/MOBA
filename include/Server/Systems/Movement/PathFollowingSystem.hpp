#pragma once

#include "Server/ServerWorld.hpp"
#include "Common/Debug/debug.hpp"

class PathFollowingSystem {
public:
    void update(ServerWorld& world) {
        world.queryColumns<Transform, Path, MovementSpeed>(
            [](std::span<Transform> t_s, std::span<Path> p_s, std::span<MovementSpeed> ms_s){
                for (size_t i = 0; i< t_s.size(); i++) {

                    auto& t = t_s[i];
                    auto& p = p_s[i];
                    auto& ms = ms_s[i];

                    if (p.active == false) {
                        continue;
                    }
                    if (p.num_waypoints > 1) {
                        DEBUG_LOG("Multiple waypoint path not implemented");
                    }
                    auto& target = p.waypoints[p.target_waypoint];\
                    auto& pos = t.position;
                    WorldSpacePos to_point = target - WorldSpacePos({pos.x, pos.y});
                    if (to_point.length() < 0.001) {    // Avoid div by 0 explosion
                        continue;
                    }
                    WorldSpacePos norm_to_point = to_point/to_point.length();
                    WorldSpacePos movement = norm_to_point * ms.speed;
                    if (movement.length() > to_point.length()) {
                        movement = to_point;
                        p.active = false;
                        // Should carry over next distance into next point
                    }
                    t.position.x += movement.x;
                    t.position.y += movement.y;
                }
            }
        );
    }
};