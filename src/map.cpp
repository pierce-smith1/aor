#include "map.h"
#include "gamewindow.h"
#include "slot/locationslot.h"

MapTile::MapTile(MapTileType type)
    : type(type) {}

MapTile::MapTile(const LocationDefinition &def)
    : type(MapTileLocation), def(def) {}

WorldMap::WorldMap() {
    m_known_locations.insert(LocationDefinition::get_def("stochastic_forest").id);

    Coord start = coord_of(*m_known_locations.begin());
    m_reveal_order.push_back(start);

    MapMask discover_mask {};
    while (auto coord = WorldMap::scan_from(start.first, start.second, 1, {}, discover_mask)) {
        m_reveal_order.push_back(*coord);
    }
}

bool WorldMap::tile_discovered(size_t y, size_t x) {
    return std::any_of(m_reveal_order.begin(), m_reveal_order.begin() + m_reveal_progress, [=](const Coord &c) {
        return c.first == y && c.second == x;
    });
}

AorUInt &WorldMap::reveal_progress() {
    return m_reveal_progress;
}

std::optional<WorldMap::Coord> WorldMap::scan_from(size_t y, size_t x, size_t depth, std::set<Coord> seen_this_scan, MapMask &discover_mask) {
    std::optional<Coord> discovered_coord;

    if (!discover_mask[y][x]) {
        discovered_coord = std::optional<Coord>({ y, x });
    }

    discover_mask[y][x] = true;
    seen_this_scan.insert({ y, x });

    if (depth == 0) {
        return discovered_coord;
    }

    for (const auto &pair : neighbors(y, x)) {
        if (seen_this_scan.find(pair) != seen_this_scan.end()) {
            continue;
        }

        if (!discover_mask[pair.first][pair.second]) {
            depth--;
        }

        if (auto coord = scan_from(pair.first, pair.second, depth, seen_this_scan, discover_mask))  {
            return coord;
        }
    }

    return discovered_coord;
}

bool WorldMap::path_exists_between(const Coord &from, const Coord &to, std::set<Coord> seen_this_scan) {
    if (from == to) {
        return true;
    }

    for (const Coord &neighbor : neighbors(from.first, from.second)) {
        if (neighbor == to) {
            return true;
        }

        if (map_tiles()[neighbor.first][neighbor.second].type == MapTileLocation) {
            continue;
        }

        if ((std::find(m_reveal_order.begin(), m_reveal_order.end(), neighbor) - m_reveal_order.begin()) > (AorInt) m_reveal_progress) {
            continue;
        }

        if (seen_this_scan.find(neighbor) == seen_this_scan.end()) {
            seen_this_scan.insert(neighbor);
            if (path_exists_between(neighbor, to, seen_this_scan)) {
                return true;
            }
        }
    }

    return false;
}

const std::vector<WorldMap::Coord> &WorldMap::reveal_order() {
    return m_reveal_order;
}

bool WorldMap::path_exists_between(LocationId from, LocationId to) {
    return path_exists_between(coord_of(from), coord_of(to));
}

#define _EMPTY_ MapTile()
#define  E__DLT  MapTile(MapTileEdgeDownLeft)
#define  EHORZT  MapTile(MapTileEdgeHorizontal)
#define  E_RD_T  MapTile(MapTileEdgeRightDown)
#define  E_RDLT  MapTile(MapTileEdgeRightDownLeft)
#define  EU__LT  MapTile(MapTileEdgeUpLeft)
#define  EVERTT  MapTile(MapTileEdgeVertical)
#define  EU_DLT  MapTile(MapTileEdgeUpDownLeft)
#define  EUR__T  MapTile(MapTileEdgeUpRight)
#define  EUR_LT  MapTile(MapTileEdgeUpRightLeft)
#define  EURD_T  MapTile(MapTileEdgeUpRightDown)
#define  ECROST  MapTile(MapTileEdgeCross)
#define L(n)     MapTile(LocationDefinition::get_def(n))

const Tiles &WorldMap::map_tiles() {
    static Tiles tiles = {{
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
        { _EMPTY_, L("JF"),  EHORZT, L("JC"), _EMPTY_, L("MO"),  E__DLT, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, L("TR"), },
        { _EMPTY_,  EVERTT, _EMPTY_,  EVERTT, _EMPTY_, _EMPTY_,  EURD_T,  E_RDLT, L("MF"),  E__DLT, _EMPTY_,  EVERTT, },
        { _EMPTY_,  EVERTT, _EMPTY_,  EVERTT, _EMPTY_,  E_RD_T, L("MZ"),  EVERTT, _EMPTY_,  EVERTT, _EMPTY_,  EVERTT, },
        { _EMPTY_,  EUR__T, L("JT"),  ECROST,  EHORZT,  EU__LT, _EMPTY_,  EVERTT, _EMPTY_,  EVERTT, _EMPTY_,  EVERTT, },
        { _EMPTY_, _EMPTY_, _EMPTY_,  EVERTT, _EMPTY_, _EMPTY_, _EMPTY_, L("MT"), _EMPTY_,  EUR__T,  EHORZT,  EU__LT, },
        { _EMPTY_, _EMPTY_, _EMPTY_, L("JH"), _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
    }};

    return tiles;
}

bool WorldMap::is_oob(size_t y, size_t x) {
    return y >= MAP_HEIGHT && x >= MAP_WIDTH;
}

std::vector<WorldMap::Coord> WorldMap::neighbors(size_t y, size_t x) {
    std::map<size_t, Coord> neighbors = {
        { 0b1000, { y + 1, x }},
        { 0b0001, { y, x + 1 }},
        { 0b0010, { y - 1, x }},
        { 0b0100, { y, x - 1 }},
    };
    std::vector<Coord> valid_neighbors;

    for (const auto &pair : neighbors) {
        Coord c = pair.second;

        if (is_oob(c.first, c.second) || map_tiles()[c.first][c.second].type == MapTileEmpty) {
            continue;
        }

        if (!(map_tiles()[c.first][c.second].type & pair.first) && !(map_tiles()[c.first][c.second].type == MapTileLocation)) {
            continue;
        }

        valid_neighbors.push_back(c);
    }

    std::shuffle(valid_neighbors.begin(), valid_neighbors.end(), *Generators::rng());

    return valid_neighbors;
}

WorldMap::Coord WorldMap::coord_of(LocationId id) {
    for (size_t y = 0; y < MAP_HEIGHT; y++) {
        for (size_t x = 0; x < MAP_WIDTH; x++) {
            if (map_tiles()[y][x].def.id == id) {
                return { y, x };
            }
        }
    }

    bugcheck(CoordsByLocationIdLookupMiss, id);
    return {};
}

void WorldMap::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, m_known_locations);
    Serialize::serialize(dev, m_reveal_order);
    Serialize::serialize(dev, m_reveal_progress);
}

MapViewTile::MapViewTile(size_t y, size_t x)
    : m_y(y), m_x(x),
      m_slot(new LocationSlot(WorldMap::map_tiles()[y][x].def, this)),
      m_image_label(new QLabel(this))
{
    setAttribute(Qt::WA_Hover);
    setMinimumSize(32, 32);
    setMaximumSize(32, 32);
    m_image_label->setMinimumSize(32, 32);
    m_image_label->setMaximumSize(32, 32);
}

void MapViewTile::refresh() {
    if (!gw()->game()->map().tile_discovered(m_y, m_x)) {
        m_slot->hide();
        m_image_label->setPixmap(QPixmap(":/assets/img/map/unknown.png"));
        m_image_label->show();
    } else if (WorldMap::map_tiles()[m_y][m_x].type & MapTileEdge) {
        m_slot->hide();

        MapTileType type = WorldMap::map_tiles()[m_y][m_x].type;
        QString tile_image_name = QString(":/assets/img/map/%1-%2-%3-%4.png")
            .arg((type & 0b1000) >> 3)
            .arg((type & 0b100) >> 2)
            .arg((type & 0b10) >> 1)
            .arg(type & 0b1);

        m_image_label->setPixmap(QPixmap(tile_image_name));
        m_image_label->show();
    } else if (WorldMap::map_tiles()[m_y][m_x].type & MapTileLocation) {
        m_slot->show();
        m_image_label->hide();
    }
}

MapView::MapView() {
    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    for (AorUInt y = 0; y < MAP_HEIGHT; y++) {
        for (AorUInt x = 0; x < MAP_WIDTH; x++) {
            MapViewTile *tile = new MapViewTile(y, x);
            layout->addWidget(tile, y, x);
            m_tiles.push_back(tile);
        }
    }
}

void MapView::refresh() {
    for (MapViewTile *tile : m_tiles) {
        tile->refresh();
    }
}
