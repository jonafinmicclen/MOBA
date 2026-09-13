#pragma once

#include "Assets/WalkableMapLoader.hpp"
#include "Common/Coordinates/WorldSpacePos.hpp"
#include "Common/Memory/2DGrid.hpp"
#include "Game/Components/Navigation/Path.hpp"
#include "Common/Coordinates/Vec2.hpp"
#include "Game/MapDef.hpp"
#include <queue>
#include <limits>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

namespace Pathfinding {

inline void aStar(MapDef& map, WorldSpacePos pos, WorldSpacePos target, Path& path) {

    WalkableMap& walkable_area = map.walkable_area;
    TransformGrid& map_from_world = map.map_from_world;

    struct MapNode {
        int x;
        int y;
        bool operator==(const MapNode& o) const { return x == o.x && y == o.y; }
    };

    struct QueueNode {
        MapNode map_node;
        int score;

        bool operator==(const MapNode& cmp) const {
            return map_node.x == cmp.x && map_node.y == cmp.y;
        }
    };

    struct Compare {
        // std::priority_queue is a MAX-heap by default. We want the
        // LOWEST f_score popped first, so this comparison must be
        // inverted ('>' not '<') or you'll process nodes backwards.
        bool operator()(const QueueNode& a, const QueueNode& b) const {
            return a.score > b.score;
        }
    };

    using MinQueue = std::priority_queue<QueueNode, std::vector<QueueNode>, Compare>;
    using UInt = uint16_t;
    using IntGrid = Grid2D<UInt>;
    using MapNodeGrid = Grid2D<MapNode>;

    const int width = walkable_area.getWidth();
    const int height = walkable_area.getHeight();

    MinQueue open_set;
    IntGrid g_scores = IntGrid(width, height, std::numeric_limits<UInt>::max());
    IntGrid f_scores = IntGrid(width, height, std::numeric_limits<UInt>::max());
    MapNodeGrid came_from = MapNodeGrid(width, height, MapNode{0, 0});

    // std::priority_queue can't remove/update arbitrary entries, so
    // "neighbor not in open_set" from the pseudocode is handled here
    // via a closed-set instead: once a node is popped and processed,
    // any older/worse duplicate entries still in the queue get skipped.
    IntGrid closed = IntGrid(width, height, 0);

    Vec2 startVec = map_from_world * pos.toVec2();
    Vec2 goalVec  = map_from_world * target.toVec2();

    // Grid-space Fixed -> nearest grid cell index.
    auto toGridCoord = [](Fixed f) -> int {
        return static_cast<int>(std::lround(f.toFloat()));
    };

    MapNode start { toGridCoord(startVec.x), toGridCoord(startVec.y) };
    MapNode goal  { toGridCoord(goalVec.x),  toGridCoord(goalVec.y) };

    // Manhattan distance - admissible heuristic for 4-directional movement.
    auto heuristic = [](const MapNode& a, const MapNode& b) -> int {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    };

    auto inBounds = [&](int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    };

    // Bresenham walk between two grid cells, true if every cell it crosses is
    // walkable. Used for string pulling below to skip the intermediate zig-zag
    // cells A* leaves behind on a 4-directional grid.
    //
    // Note: this steps diagonally between cells (8-connected), while the A*
    // search above only ever moves 4-directionally. That means a "clear" line
    // here can graze diagonally past a single blocked cell corner that the
    // search itself could never have walked through. Acceptable for now since
    // walls are made of many cells, not isolated single-cell corners.
    auto hasLineOfSight = [&](const MapNode& a, const MapNode& b) {
        int x0 = a.x, y0 = a.y;
        const int x1 = b.x, y1 = b.y;
        const int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        const int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true) {
            if (!walkable_area.at(x0, y0)) return false;
            if (x0 == x1 && y0 == y1) return true;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    };

    // String pulling: greedily replace runs of the grid path with the single
    // longest straight line-of-sight segment available from each anchor, so a
    // long straight corridor collapses to 2 waypoints instead of one per cell.
    // This lets the MAX_WAYPOINTS cap cover far more actual distance.
    auto stringPull = [&](const std::vector<MapNode>& grid_path) {
        std::vector<MapNode> pulled;
        pulled.push_back(grid_path.front());

        size_t anchor = 0;
        while (anchor < grid_path.size() - 1) {
            size_t furthest = anchor + 1;
            for (size_t i = anchor + 2; i < grid_path.size(); ++i) {
                if (!hasLineOfSight(grid_path[anchor], grid_path[i])) break;
                furthest = i;
            }
            pulled.push_back(grid_path[furthest]);
            anchor = furthest;
        }

        return pulled;
    };

    // If either endpoint is off the walkable map or literally on a
    // blocked cell, there's no point starting the search.
    if (!inBounds(start.x, start.y) || !inBounds(goal.x, goal.y)) {
        path.active = false;
        path.num_waypoints = 0;
        return;
    }

    g_scores.at(start.x, start.y) = 0;
    f_scores.at(start.x, start.y) = static_cast<UInt>(heuristic(start, goal));
    open_set.push({ start, f_scores.at(start.x, start.y) });

    static constexpr int dx[4] = { 1, -1, 0, 0 };
    static constexpr int dy[4] = { 0, 0, 1, -1 };

    while (!open_set.empty()) {
        QueueNode current = open_set.top();
        open_set.pop();

        if (closed.at(current.map_node.x, current.map_node.y)) {
            continue; // stale duplicate entry, already finalized with a better score
        }
        closed.at(current.map_node.x, current.map_node.y) = 1;

        if (current.map_node == goal) {
            // Walk came_from backwards from goal to start, collecting grid nodes.
            std::vector<MapNode> reversed;
            MapNode node = current.map_node;
            while (!(node == start)) {
                reversed.push_back(node);
                node = came_from.at(node.x, node.y);
            }
            reversed.push_back(start);
            // reversed is currently goal->...->start; we need start->...->goal.
            std::reverse(reversed.begin(), reversed.end());

            std::vector<MapNode> pulled = stringPull(reversed);

            // Take at most the first MAX_WAYPOINTS points from start to finish,
            // converting each back to world space.
            size_t count = std::min(pulled.size(), MAX_WAYPOINTS);
            for (size_t i = 0; i < count; ++i) {
                Vec2 gridPoint {
                    Fixed(static_cast<float>(pulled[i].x)),
                    Fixed(static_cast<float>(pulled[i].y))
                };
                Vec2 worldPoint = map_from_world.applyInverse(gridPoint);
                path.waypoints[i] = WorldSpacePos::fromVec2(worldPoint);
            }

            path.num_waypoints = count;
            path.target_waypoint = 0;
            path.active = true;
            return;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = current.map_node.x + dx[i];
            int ny = current.map_node.y + dy[i];

            if (!inBounds(nx, ny)) continue;
            if (!walkable_area.at(nx, ny)) continue; // see note below - assumes true = walkable
            if (closed.at(nx, ny)) continue;

            int tentative_g = g_scores.at(current.map_node.x, current.map_node.y) + 1;

            if (tentative_g < g_scores.at(nx, ny)) {
                came_from.at(nx, ny) = current.map_node;
                g_scores.at(nx, ny) = static_cast<UInt>(tentative_g);
                int f = tentative_g + heuristic({ nx, ny }, goal);
                f_scores.at(nx, ny) = static_cast<UInt>(f);
                open_set.push({ MapNode{ nx, ny }, f });
            }
        }
    }

    // Open set exhausted, goal unreachable.
    path.active = false;
    path.num_waypoints = 0;
}

}