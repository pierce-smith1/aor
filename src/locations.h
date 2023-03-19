#pragma once

#include <array>

#include "types.h"
#include "definitionmark.h"
#include "items.h"

#define CONSUMABLES
#define MINEABLES

const static AorUInt WEATHER_EFFECTS = 2;
const static LocationId NOWHERE = 0;

enum Biome : AorUInt {
    BiomeNone   = 0,
    BiomeJungle = 1 << 8,
};

struct LocationDefinition {
    LocationId id;
    QString internal_name;
    QString display_name;
    QString two_letter_code;
    AorUInt consumables;
    AorUInt mineables;
    ItemProperties properties;

    static LocationDefinition get_def(LocationId id);
    static LocationDefinition get_def(const QString &name_or_code);
};

const static std::vector<LocationDefinition> LOCATION_DEFINITIONS = WeatherMark::resolve_markers(ITEM_DEFINITIONS, {
    {
        NOWHERE,
        "nowhere", "Nowhere", "XX",
        0 CONSUMABLES, 0 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("empty") },
            { WeatherEffect2, WeatherMark::marker("empty") },
        }
    },
    {
        BiomeJungle | 0,
        "stochastic_forest", "Stochastic Forest", "JF",
        15 CONSUMABLES, 5 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeJungle | 1,
        "chameleon_cluster", "Chameleon Cluster", "JC",
        20 CONSUMABLES, 0 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    }
});
