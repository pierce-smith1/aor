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
        "registry_hive", "Registry Hive", "JH",
        "<i>These busy bees have built quite the hive!</i><br>"
        "<i>They use cube honeycombs, naturally -</i><br>"
        "<i>some are filled with delicious honey, but most are just filled with smaller hives,</i><br>"
        "<i>buzzing with smaller and smaller bees.</i><br>",
        15 FORAGEABLES, 10 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("acid_stingers") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 10 },
            { LocationEnergyCost, 10 },
            { LocationSignatureItem1, WeatherMark::marker("honeykey") },
        }
    },
    {
        BiomeJungle | 1,
        "Mycrocelium Oasis", "Mycrocelium Oasis", "JC",
        "<i>Visible from any outlook, powerful points of azure light beam from</i><br>"
        "<i>luminescent mushrooms in the jungle below.</i><br>"
        "<i>Word around here is that these fungal growths excel wherever there's access to</i><br>"
        "<i>at least 365 days of rain from the clouds.</i><br>",
        30 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("unaligned_terrain") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 10 },
            { LocationEnergyCost, 10 },
            { LocationSignatureItem1, WeatherMark::marker("fungified_token") },
            { LocationSignatureItem2, WeatherMark::marker("polymore") },
            { LocationSignatureItem3, WeatherMark::marker("copycap") },
        }
    },
    {
        BiomeJungle | 2,
        "sigtermite_nest", "Sigtermite Nest", "JT",
        "<i>Sigtermites are responsible for cleaning up those who misbehave;</i><br>"
        "<i>just don't kick the nests, don't step on the plants, and you won't be terminated.</i><br>",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("detritus_reapers") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 10 },
            { LocationEnergyCost, 10 },
            { LocationSignatureItem1, WeatherMark::marker("binary_pesticide") },
        }
    },
    {
        BiomeJungle | 3,
        "point_entry", "Point Entry", "JE",
        "<i>If you start me up</i><br>"
        "<i>If you start me up, I'll never stop</i><br>",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("empty") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 5 },
            { LocationEnergyCost, 5 },
            { LocationSignatureItem1, WeatherMark::marker("virtualized_fruit") }
        }
    },
    {
        BiomeMesa | 0,
        "pentium_mines", "Pentium Mines", "MZ",
        "<i>While some of Rhodon's fledgling natives choose the much more questionably effective Xeon-Itanium alloys,</i><br>"
        "<i>the flexible and powerful Pentium remains a crucial metal in nearly everything they build.</i><br>"
        "<i>Luckily, these creatures seem to be deathly afraid of us, leaving us to use their mines at will -</i><br>"
        "<i>a near limitless source of this valuable metal.</i><br>",
        10 FORAGEABLES, 50 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("unstable_division") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 15 },
            { LocationSignatureItem1, WeatherMark::marker("pentium_bar") },
            { LocationSignatureItem2, WeatherMark::marker("pentium_bar") },
            { LocationSignatureItem3, WeatherMark::marker("pentium_bar") },
            { LocationSignatureItem4, WeatherMark::marker("pentium_bar") },
            { LocationSignatureItem5, WeatherMark::marker("pentium_bar") },
        }
    },
    {
        BiomeMesa | 1,
        "rocking_obelisk", "Rocking Obelisk", "MO",
        "<i>An obsilicon monument dedicated to worship of that most holy panacea;</i><br>"
        "<i>turning it off, then back on again.</i><br>",
        30 FORAGEABLES, 30 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("unified_egg_firmware_interface") },
            { WeatherEffect2, WeatherMark::marker("unaligned_terrain") },
            { LocationSpiritCost, 30 },
            { LocationSignatureItem1, WeatherMark::marker("advanced_coniine_and_phoratoxin_iris") },
        }
    },
    {
        BiomeMesa | 2,
        "the_southbridge", "The Southbridge", "MB",
        "<i>At one point, the sandy Southbridge led to an ancient city of vaults -</i><br>"
        "<i>now crumbling and half-swallowed by the desert, this is obviously no longer the case.</i><br>"
        "<i>The precious artifacts have been seemingly moved elsewhere, but maybe there's a</i><br>"
        "<i>trace of something in the ruins yet.</i><br>",
        15 FORAGEABLES, 30 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("nonvolatile_compounds") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 30 },
            { LocationResourceRequirement, 40 },
            { LocationSignatureItem1, WeatherMark::marker("amniotic_cable") },
        }
    },
    {
        BiomeMesa | 3,
        "page_fault", "Page Fault", "MF",
        "<i>This great fracture in the earth is the result of several tectonic frames sliding past one another.</i><br>"
        "<i>Far down enough, the chasm expands into a bottomless segmentation fault...</i><br>"
        "<i>there's no crawling out of that one.</i><br>",
        20 FORAGEABLES, 20 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("unaligned_terrain") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 30 },
            { LocationEnergyCost, 30 },
            { LocationSignatureItem1, WeatherMark::marker("corrupting_nematode") }
        }
    },
    {
        BiomeRoses | 0,
        "tree_of_roses", "The Tree of Roses", "TR",
        "<i>From the cracks in the sky grows the Tree of Roses - the misty citadel of</i><br>"
        "<i>steel and petal that holds the line between the natural and the abstract.</i><br>"
        "<i>Our existence here strains the Tree, and its ours...</i><br>"
        "<i>but we are here to make this home, so we must persevere.</i><br>",
        0 FORAGEABLES, 0 MINEABLES,
        {
            { WeatherEffect1, WeatherMark::marker("empty") },
            { WeatherEffect2, WeatherMark::marker("empty") },
            { LocationSpiritCost, 100 },
            { LocationEnergyCost, 100 },
            { LocationPartyRequirement, 6 },
            { LocationResourceRequirement, 250 },
        }
    }
});
