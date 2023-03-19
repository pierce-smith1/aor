#pragma once

#include <set>
#include <vector>
#include <array>
#include <optional>

#include <QWidget>

#include "locations.h"
#include "types.h"

class LocationSlot;

const static AorUInt MAP_WIDTH = 20;
const static AorUInt MAP_HEIGHT = 10;

enum MapTileType {
    MapTileEmpty,
    MapTileEdge = 0b10000,
    MapTileEdgeDownLeft = MapTileEdge | 0b0011,
    MapTileEdgeHorizontal = MapTileEdge | 0b0101,
    MapTileEdgeRightDown = MapTileEdge | 0b0110,
    MapTileEdgeRightDownLeft = MapTileEdge | 0b0111,
    MapTileEdgeUpLeft = MapTileEdge | 0b1001,
    MapTileEdgeVertical = MapTileEdge | 0b1010,
    MapTileEdgeUpDownLeft = MapTileEdge | 0b1011,
    MapTileEdgeUpRight = MapTileEdge | 0b1100,
    MapTileEdgeUpRightLeft = MapTileEdge | 0b1101,
    MapTileEdgeUpRightDown = MapTileEdge | 0b1110,
    MapTileEdgeCross = MapTileEdge | 0b1111,
    MapTileLocation,
};

struct MapTile {
    MapTile() = default;
    MapTile(MapTileType type);
    MapTile(const LocationDefinition &location);

    MapTileType type = MapTileEmpty;
    LocationDefinition def = LocationDefinition::get_def(NOWHERE);
};

using Tiles = std::array<std::array<MapTile, MAP_WIDTH>, MAP_HEIGHT>;

class WorldMap {
public:
    using Coord = std::pair<size_t, size_t>;

    WorldMap();

    bool tile_discovered(size_t y, size_t x);
    void scan_from(size_t y, size_t x, size_t depth);

    void serialize(QIODevice *dev) const;
    static WorldMap deserialize(QIODevice *dev);

    static const Tiles &map_tiles();

private:
    bool is_oob(size_t y, size_t x);
    void scan_paths_from(size_t y, size_t x, size_t depth);
    std::vector<Coord> neighbors(size_t y, size_t x);
    Coord coord_of(LocationId location_id);

    std::set<LocationId> m_known_locations;
    std::array<std::array<bool, MAP_WIDTH>, MAP_HEIGHT> m_tile_discovered {};
};

class MapViewTile : public QWidget {
public:
    MapViewTile(size_t y, size_t x);

    void refresh();

private:
    size_t m_y, m_x;
    LocationSlot *m_slot;
    QLabel *m_image_label;
};

class MapView : public QWidget {
public:
    MapView();

    void refresh();

private:
    std::vector<MapViewTile *> m_tiles;
};
