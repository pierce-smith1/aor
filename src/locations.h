#pragma once

#include <array>

#include "types.h"
#include "definitionmark.h"
#include "items.h"

#define FORAGEABLES
#define MINEABLES

const static AorUInt WEATHER_EFFECTS = 2;
const static LocationId NOWHERE = 0;

enum Biome : AorUInt {
    BiomeNone   = 0,
    BiomeJungle = 1 << 8,
    BiomeMesa = 1 << 9,
    BiomeRoses = 1 << 10,
};

struct LocationDefinition {
    LocationId id;
    QString internal_name;
    QString display_name;
    QString two_letter_code;
    QString description;
    AorUInt forageables;
    AorUInt mineables;
    ItemProperties properties;

    static LocationDefinition get_def(LocationId id);
    static LocationDefinition get_def(const QString &name_or_code);
};

const static std::vector<LocationDefinition> LOCATION_DEFINITIONS = WeatherMark::resolve_markers(ITEM_DEFINITIONS, {
    {
        NOWHERE,
        "nowhere", "Nowhere", "XX",
        "Nowhere",
        0 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("empty") },
            { WeatherEffect2, WeatherMark::marker("empty") },
        }
    },
    {
        BiomeJungle | 0,
        "stochastic_forest", "Stochastic Forest", "JF",
        "<i>There's a constant sound of cracking bark as the very earth shifts under my feet...</i><br>"
        "<i>The place the soil sinks seems to be always moving around.</i><br>"
        "<i>It really makes us see the forest for the trees.</i><br>",
        15 FORAGEABLES, 5 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeJungle | 1,
        "chameleon_cluster", "Chameleon Cluster", "JC",
        "My own feathers are changing color!",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeJungle | 2,
        "jungle_2", "Jungle in Time", "JT",
        "",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeJungle | 3,
        "jungle_3", "jungle_3", "JH",
        "",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeMesa | 0,
        "mesa_0", "mesa_0", "MZ",
        "",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeMesa | 1,
        "mesa_1", "mesa_1", "MO",
        "",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeMesa | 2,
        "mesa_2", "mesa_2", "MT",
        "",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeMesa | 3,
        "mesa_3", "mesa_3", "MF",
        "",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
    {
        BiomeRoses | 0,
        "tree_of_roses", "The Tree of Roses", "TR",
        "<i>From the cracks in the sky grows the Tree of Roses - the citadel of</i><br>"
        "<i>steel and petal that holds the line between concrete and abstract.</i><br>"
        "<i>We give everything to the Tree while we are here; we are all its children.</i><br>"
        "<i>For now.</i><br>",
        10 FORAGEABLES, 0 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_rain") },
            { WeatherEffect2, WeatherMark::marker("worm_storms") },
        }
    },
});
