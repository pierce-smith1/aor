#pragma once

#include <set>
#include <vector>

enum Biome {
    Wild,
    Mesa,
};

struct Location {
    Biome biome;
};

struct MapNode {
    explicit MapNode(MapNode *parent);
    ~MapNode();

    std::vector<MapNode *> children;
    std::size_t index_in_parent;
    MapNode *parent;
};

class WorldMap {
public:
    WorldMap();
private:
    MapNode *m_root;
};
