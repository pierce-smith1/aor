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
    m_tile_discovered[start.first][start.second] = true;
}

bool WorldMap::tile_discovered(size_t y, size_t x) {
    return m_tile_discovered[y][x];
}

void WorldMap::scan_from(size_t y, size_t x, size_t depth) {
    m_tile_discovered[y][x] = true;

    if (depth == 0) {
        return;
    }

    for (auto pair : neighbors(y, x)) {
        if (m_tile_discovered[pair.first][pair.second]) {
            continue;
        }

        scan_paths_from(pair.first, pair.second, depth - 1);
    }
}

#define _EMPTY_ MapTile()
#define  E__DL  MapTile(MapTileEdgeDownLeft)
#define  EHORZ  MapTile(MapTileEdgeHorizontal)
#define  E_RD_  MapTile(MapTileEdgeRightDown)
#define  E_RDL  MapTile(MapTileEdgeRightDownLeft)
#define  EU__L  MapTile(MapTileEdgeUpLeft)
#define  EVERT  MapTile(MapTileEdgeVertical)
#define  EU_DL  MapTile(MapTileUpDownLeft)
#define  EUR__  MapTile(MapTileEdgeUpRight)
#define  EUR_L  MapTile(MapTileEdgeUpRightLeft)
#define  EURD_  MapTile(MapTileEdgeUpRightDown)
#define  ECROS  MapTile(MapTileEdgeCross);
#define L(n)    MapTile(LocationDefinition::get_def(n))

const Tiles &WorldMap::map_tiles() {
    static Tiles tiles = {{
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, L("JC"),  EHORZ ,  EHORZ ,  E__DL , _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_,  EVERT , _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, L("JF"), _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
        { _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, _EMPTY_, },
    }};

    return tiles;
}

bool WorldMap::is_oob(size_t y, size_t x) {
    return y >= MAP_HEIGHT && x >= MAP_WIDTH;
}

void WorldMap::scan_paths_from(size_t y, size_t x, size_t depth) {
    m_tile_discovered[y][x] = true;

    if (depth == 0) {
        return;
    }

    for (auto pair : neighbors(y, x)) {
        if (m_tile_discovered[pair.first][pair.second]) {
            continue;
        }

        if (map_tiles()[pair.first][pair.second].type == MapTileLocation) {
            m_tile_discovered[pair.first][pair.second] = true;
            continue;
        }

        scan_paths_from(pair.first, pair.second, depth - 1);
    }
}

std::vector<WorldMap::Coord> WorldMap::neighbors(size_t y, size_t x) {
    std::vector<Coord> neighbors = {
        { y + 1, x     },
        { y    , x - 1 },
        { y    , x + 1 },
        { y - 1, x     }
    };

    neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(), [&](const Coord &pair) {
        return is_oob(pair.first, pair.second) || map_tiles()[pair.first][pair.second].type == MapTileEmpty;
    }), neighbors.end());

    return neighbors;
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

MapViewTile::MapViewTile(size_t y, size_t x)
    : m_y(y), m_x(x),
      m_slot(new LocationSlot(WorldMap::map_tiles()[y][x].def, this)),
      m_image_label(new QLabel(this))
{
    setMinimumSize(32, 32);
    setMaximumSize(32, 32);
    m_image_label->setMinimumSize(32, 32);
    m_image_label->setMaximumSize(32, 32);
}

void MapViewTile::refresh() {
    if (!gw()->game().map().tile_discovered(m_y, m_x)) {
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
    } else {
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
