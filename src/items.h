#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <map>
#include <random>
#include <set>

#include <QtGlobal>
#include <QFile>
#include <QPixmap>

#include "tooltip.h"

#define USES
#define LEVEL

const static int SMITHING_SLOTS = 3;
const static int SMITHING_SLOTS_PER_ROW = 3;

const static int PRAYER_SLOTS = 6;
const static int PRAYER_SLOTS_PER_ROW = 3;

const static int ARTIFACT_SLOTS = 4;

const static int EFFECT_SLOTS = 7;

using ItemCode = std::uint16_t;
using ItemId = std::uint64_t;
using CharacterId = std::uint16_t;
using GameId = std::uint64_t;

const static ItemId EMPTY_ID = 0;
const static ItemId INVALID_ID = 0xffffffffffffffff;
const static ItemCode INVALID_CODE = 0xff;
const static CharacterId NOBODY = 0xffff;

using ItemType = std::uint16_t;
enum ItemDomain : ItemType {
    Ordinary      = 0,      None = 0,
    Consumable    = 1 << 0, Eating = 1 << 0,
    Material      = 1 << 1,
    SmithingTool  = 1 << 2, Smithing = 1 << 2,
    ForagingTool  = 1 << 3, Foraging = 1 << 3,
    MiningTool    = 1 << 4, Mining = 1 << 4,
    PrayerTool    = 1 << 5, Praying = 1 << 5,
    Blessing      = 1 << 6,
    Artifact      = 1 << 7,
    Effect        = 1 << 8,
    Rune          = 1 << 9,
    Portrait      = 1 << 10,
    Offering      = 1 << 11,
    KeyOffering   = Offering | 1 << 12,
    Tool          = SmithingTool | ForagingTool | MiningTool | PrayerTool
};

const static int CT_EMPTY = 0;
const static int CT_CONSUMABLE = 1 << 8;
const static int CT_MATERIAL = 1 << 9;
const static int CT_TOOL = 1 << 10;
const static int CT_ARTIFACT = 1 << 11;
const static int CT_EFFECT = 1 << 12;
const static int CT_RUNE = 1 << 13;
const static int CT_OTHER = 1 << 14;

enum ItemProperty {
    ToolEnergyCost,
    SpeedBonus,
    // WARNING: Item generation behavior in actions.cpp requires that there
    // are exactly 9 ToolCanDiscovers, exactly 9 ToolDiscoverWeights, and
    // that the ToolDiscoverWeights comes directly after the ToolCanDiscovers!
    // If you don't like that, fix it in actions.cpp.
    ToolCanDiscover1,
    ToolCanDiscover2,
    ToolCanDiscover3,
    ToolCanDiscover4,
    ToolCanDiscover5,
    ToolCanDiscover6,
    ToolCanDiscover7,
    ToolCanDiscover8,
    ToolCanDiscover9,
    ToolDiscoverWeight1,
    ToolDiscoverWeight2,
    ToolDiscoverWeight3,
    ToolDiscoverWeight4,
    ToolDiscoverWeight5,
    ToolDiscoverWeight6,
    ToolDiscoverWeight7,
    ToolDiscoverWeight8,
    ToolDiscoverWeight9,
    ToolComboIngredient1,
    ToolComboIngredient2,
    ToolComboIngredient3,
    ToolComboResult,
    ConsumableEnergyBoost,
    ConsumableMoraleBoost,
    ConsumableGivesEffect,
    ConsumableClearsNumEffects,
    MaterialForges,
    ArtifactMaxEnergyBoost,
    ArtifactMaxMoraleBoost,
    ArtifactSpeedBonus,
};

// This basically just wraps a std::map<ItemPropety, int>,
// with the crucial change that operator[] is const while retaining the
// behavior of returning zero-initialized values for non-existant keys.
class ItemProperties {
public:
    ItemProperties(std::initializer_list<std::pair<const ItemProperty, std::uint16_t>> map);
    std::uint16_t operator[](ItemProperty prop) const;

    std::map<ItemProperty, std::uint16_t> map;
};

struct ItemDefinition {
    ItemCode code;
    QString internal_name;
    QString display_name;
    QString description;
    unsigned char default_uses_left;
    ItemType type;
    int item_level;
    ItemProperties properties;
};

using ItemDefinitionPtr = std::vector<ItemDefinition>::const_iterator;

const static std::vector<ItemDefinition> ITEM_DEFINITIONS = {
    {
        CT_EMPTY,
        "empty", "Empty",
        "Nothing here.",
        0 USES, Ordinary, LEVEL 0,
        {}
    },
    {
        CT_CONSUMABLE | 0,
        "globfruit", "Globfruit",
        "<i>A relative of the starfruit, this one is a lot stickier.</i>",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 20 },
        }
    },
    {
        CT_CONSUMABLE | 1,
        "byteberry", "Byteberry",
        "<i>Grown on the leaves of tries, these make an excellent treat when lightly charred.</i>",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 10 },
            { ConsumableMoraleBoost, 10 }
        }
    },
    {
        CT_MATERIAL | 0,
        "obsilicon", "Obsilicon",
        "<i>This glassy stone cooled from the same primordial magma<br>"
        "that birthed our wafer-thing planes of reality.</i>",
        1 USES, Material, LEVEL 1,
        {
            { MaterialForges, CT_TOOL | 0 }
        }
    },
    {
        CT_MATERIAL | 1,
        "oolite", "Oolite",
        "<i>You've heard small talk from geologists that this may<br>"
        "have been the first rock ever constructed by the earth.</i>",
        1 USES, Material, LEVEL 1,
        {
            { MaterialForges, CT_TOOL | 1 }
        }
    },
    {
        CT_TOOL | 0,
        "maven_mallet", "Maven Mallet",
        "<i>A rudimentary tool. You've named it after the maven, a strange<br>"
        "little bird you've seen around here that tells you long-winded stories.</i>",
        3 USES, SmithingTool, LEVEL 1,
        {
            { ToolEnergyCost, 20 },
            { ToolComboIngredient1, CT_MATERIAL | 0 },
            { ToolComboIngredient2, CT_MATERIAL | 1 },
            { ToolComboIngredient3, CT_MATERIAL | 1 },
            { ToolComboResult, CT_TOOL | 2 }
        }
    },
    {
        CT_TOOL | 1,
        "hashcracker", "Hashcracker",
        "<i>Rhodon is covered in layers of hard shale-256 - but with a good axe like this,<br>"
        "you should be able to brute-force your way through it to find the goodies underneath.</i>",
        4 USES, ForagingTool, LEVEL 1,
        {
            { ToolEnergyCost, 20 },
            { ToolCanDiscover1, CT_CONSUMABLE | 2 },
            { ToolCanDiscover2, CT_CONSUMABLE | 3 },
            { ToolCanDiscover3, CT_MATERIAL | 3 },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 },
            { ToolDiscoverWeight3, 1 },
        }
    },
    {
        CT_TOOL | 2,
        "basalt_destructor", "Basalt Destructor",
        "<i>It's just plain old dirt - this will be good enough.</i>",
        4 USES, MiningTool, LEVEL 2,
        {
            { ToolEnergyCost, 20 },
            { ToolCanDiscover1, CT_MATERIAL | 2 },
            { ToolCanDiscover2, CT_MATERIAL | 3 },
            { ToolCanDiscover3, CT_MATERIAL | 4 },
            { ToolDiscoverWeight1, 2 },
            { ToolDiscoverWeight2, 2 },
            { ToolDiscoverWeight3, 3 },
        }
    },
    {
        CT_CONSUMABLE | 2,
        "norton_ghost_pepper", "Norton Ghost Pepper",
        "<i>Haunted with a benevolent spirit that will cure your ailments.<br>"
        "Often found growing in places you didn't ask them to.</i>",
        1 USES, Consumable, LEVEL 2,
        {
            { ConsumableClearsNumEffects, 1 }
        }
    },
    {
        CT_CONSUMABLE | 3,
        "bleeding_krazaheart", "Bleeding Krazaheart",
        "<iDon't read too far into it, Kraza is fine.</i>",
        1 USES, Consumable, LEVEL 2,
        {
            { ConsumableEnergyBoost, 30 },
            { ConsumableMoraleBoost, 10 }
        }
    },
    {
        CT_MATERIAL | 5,
        "fireclay", "Fireclay",
        "<i>Used to build firewalls, this peculiar substance is selectively<br>"
        "porous to microbes on its surface.</i>",
        1 USES, Material, LEVEL 2,
        {}
    },
    {
        CT_MATERIAL | 2,
        "cobolt_bar", "Cobolt Bar",
        "<i>Once a foundational metal of the Rhodon world, now an outdated curiosity owned mostly by<br>"
        "the dwindling elite who still know how to mine it.</i>",
        1 USES, Material, LEVEL 2,
        {
            { MaterialForges, CT_TOOL | 3 }
        }
    },
    {
        CT_MATERIAL | 3,
        "solid_slate", "Solid Slate",
        "<i>A crucial improvement over oldschool spinning schists.</i>",
        1 USES, Material, LEVEL 2,
        {
            { MaterialForges, CT_TOOL | 4 }
        }
    },
    {
        CT_MATERIAL | 4,
        "scandiskium", "Scandiskium",
        "<i>Whenever Rhodon is torn into darkness by a Great Reroot,<br>"
        "another vein of this soft, yellowish metal is born.</i>",
        1 USES, Material, LEVEL 2,
        {
            { MaterialForges, CT_TOOL | 5 }
        }
    },
    {
        CT_TOOL | 3,
        "seaquake", "Seaquake",
        "<i>And with a thunderous clang, the great towers of<br>"
        "the sea were assembled from liquid rubble.</i>",
        4 USES, SmithingTool, LEVEL 3,
        {
            { ToolEnergyCost, 30 },
            { ToolComboIngredient1, CT_MATERIAL | 2 },
            { ToolComboIngredient1, CT_MATERIAL | 4 },
            { ToolComboIngredient1, CT_MATERIAL | 5 },
            { ToolComboResult, CT_TOOL | 6 }
        }
    },
    {
        CT_TOOL | 4,
        "disk_fragmenter", "Disk Fragmenter",
        "Yeah, this is how it happens.",
        1 USES, MiningTool, LEVEL 3,
        {
            { ToolEnergyCost, 15 },
            { ToolCanDiscover1, CT_ARTIFACT | 0 },
            { ToolCanDiscover2, CT_ARTIFACT | 1 },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 }
        }
    },
    {
        CT_TOOL | 5,
        "sepulchre_of_corruption", "Sepulchre of Corruption",
        "<i>There are some problems you simply cannot solve.</i>",
        3 USES, PrayerTool, LEVEL 3,
        {}
    },
    {
        CT_ARTIFACT | 0,
        "recovered_journal", "Recovered Journal",
        "<i>Tells a sad story of an orphan far from home.</i>",
        0 USES, Artifact, LEVEL 3,
        {
            { ArtifactMaxMoraleBoost, 20 }
        }
    },
    {
        CT_ARTIFACT | 1,
        "scalped_remains", "Scalped Remains",
        "<i>Whoever this was has been long forgotton... but is not gone.</i>",
        0 USES, Artifact, LEVEL 3,
        {
            { ArtifactMaxEnergyBoost, 20 }
        }
    }
};

struct Item {
    ItemCode code {0};
    ItemId id {EMPTY_ID};
    unsigned char uses_left {0};
    ItemDomain intent {Ordinary};
    CharacterId intent_holder {NOBODY};

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const QString &name);

    ItemDefinitionPtr def() const;

    static ItemDefinitionPtr def_of(ItemCode id);
    static ItemDefinitionPtr def_of(const QString &name);
    static ItemDefinitionPtr def_of(const Item &item);
    static QPixmap pixmap_of(ItemCode id);
    static QPixmap pixmap_of(const QString &name);
    static QPixmap pixmap_of(const ItemDefinition &def);
    static QPixmap pixmap_of(const Item &item);
    static Item invalid_item();

    static QString type_to_string(ItemType type);
    static QString properties_to_string(const ItemProperties &props);

    static Item empty_item;
};
