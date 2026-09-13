#pragma once

#include <cmath>
#include <glm/gtc/quaternion.hpp>

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

                    // Consume this tick's movement budget across as many waypoints
                    // as it covers, carrying leftover distance into the next
                    // segment instead of losing it at each corner.
                    float remaining_speed = ms.speed;
                    while (p.active && remaining_speed > 0.0f) {
                        auto& target = p.waypoints[p.target_waypoint];
                        WorldSpacePos to_point = target - WorldSpacePos({t.position.x, t.position.y});
                        float distance = to_point.length();

                        if (distance < 0.001) {
                            advanceWaypoint(p);
                            continue;
                        }

                        WorldSpacePos direction = to_point / distance;
                        faceDirection(t, direction);

                        if (distance <= remaining_speed) {
                            t.position.x += to_point.x;
                            t.position.y += to_point.y;
                            remaining_speed -= distance;
                            advanceWaypoint(p);
                        } else {
                            WorldSpacePos movement = direction * remaining_speed;
                            t.position.x += movement.x;
                            t.position.y += movement.y;
                            remaining_speed = 0.0f;
                        }
                    }
                }
            }
        );
    }

private:
    // Turns the entity to face its movement direction. Assumption: identity
    // rotation (no rotation applied) means the character model's rest pose
    // already faces world +X - this is arbitrary and unverified against any
    // real asset, per the caller's request. If a character's .glb rest pose
    // faces a different direction, that needs a per-asset corrective
    // rotation applied wherever the model is loaded/spawned (not handled
    // here), not a change to this yaw math.
    static void faceDirection(Transform& t, const WorldSpacePos& direction) {
        float yaw = std::atan2(direction.y.toFloat(), direction.x.toFloat());
        t.rotation = glm::angleAxis(yaw, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    static void advanceWaypoint(Path& p) {
        p.target_waypoint++;
        if (p.target_waypoint >= p.num_waypoints) {
            p.active = false;
        }
    }
};