#pragma once

#include <set>
#include <vector>
#include <array>
#include <optional>

#include <QWidget>

#include "locations.h"
#include "types.h"
#include "serialize.h"

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

class WorldMap : public Serializable {
public:
    using Coord = std::pair<size_t, size_t>;
    using MapMask = std::array<std::array<bool, MAP_WIDTH>, MAP_HEIGHT>;

    WorldMap();

    Coord &cursor_pos();

    bool tile_discovered(size_t y, size_t x);
    bool path_exists_between(LocationId from, LocationId to);
    AorUInt &reveal_progress();
    const std::vector<Coord> &reveal_order();

    void serialize(QIODevice *dev) const;
    static WorldMap deserialize(QIODevice *dev);

    static const Tiles &map_tiles();

private:
    std::optional<Coord> scan_from(size_t y, size_t x, size_t depth, std::set<Coord> seen_this_scan, MapMask &discover_mask);
    bool path_exists_between(const Coord &from, const Coord &to, std::set<Coord> seen_this_scan = {});
    bool is_oob(size_t y, size_t x);
    std::vector<Coord> neighbors(size_t y, size_t x);
    Coord coord_of(LocationId location_id);

    std::set<LocationId> m_known_locations;
    std::vector<WorldMap::Coord> m_reveal_order;
    AorUInt m_reveal_progress = 1;
};

class MapViewTile : public QWidget {
public:
    MapViewTile(size_t y, size_t x);

    void refresh();

private:
    size_t m_y, m_x;
    LocationSlot *m_slot;
    QLabel *m_image_label;
    bool m_hovered = false;
};

class MapView : public QWidget {
public:
    MapView();

    void refresh();

private:
    std::vector<MapViewTile *> m_tiles;
};
