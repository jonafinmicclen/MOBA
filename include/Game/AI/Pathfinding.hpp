#pragma once

#include "Assets/WalkableMapLoader.hpp"
#include "Common/Coordinates/WorldSpacePos.hpp"
#include "Common/Memory/2DGrid.hpp"
#include <queue>

namespace Pathfinding {



    inline void aStar(WalkableMap& walkable_area, WorldSpacePos pos, WorldSpacePos target) {

        struct QueueNode {
            int x;
            int y;
            int score;
        };

        struct Compare {
            bool operator()(const QueueNode& a, const QueueNode& b) {
                return a.score < b.score;
            }
        };

        using MinQueue = std::priority_queue<QueueNode, std::vector<QueueNode>, Compare>;

        MinQueue open_set;
        

    }
};