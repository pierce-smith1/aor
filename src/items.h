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
#include "generators.h"

#define USES
#define LEVEL
#define NOT_CRAFTABLE {}
#define NOT_TOOL {}

const static int INVENTORY_ROWS = 4;
const static int INVENTORY_COLS = 7;
const static int INVENTORY_SIZE = INVENTORY_COLS * INVENTORY_ROWS;

const static int SMITHING_SLOTS = 3;
const static int SMITHING_SLOTS_PER_ROW = 3;

const static int PRAYER_SLOTS = 6;
const static int PRAYER_SLOTS_PER_ROW = 3;

const static int ARTIFACT_SLOTS = 4;

const static int EFFECT_SLOTS = 7;

using ItemCode = std::uint16_t;
using ItemId = std::uint64_t;
using ForeignItemId = std::uint64_t;

const static ItemId EMPTY_ID = 0;
const static ItemId INVALID_ID = 0xffffffffffffffff;
const static ItemCode INVALID_CODE = 0xff;

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
    Character     = 1 << 10,
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
    ConsumableEnergyBoost,
    ConsumableMoraleBoost,
    ConsumableGivesEffect,
    ConsumableGivesBuff,
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
    MaterialForges,
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
        "A relative of the starfruit, this one is a lot stickier.",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 20 },
        }
    },
    {
        CT_CONSUMABLE | 1,
        "byteberry", "Byteberry",
        "Grown on the leaves of tries, these make an excellent treat when lightly charred.",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 10 },
            { ConsumableMoraleBoost, 10 }
        }
    },
    {
        CT_CONSUMABLE | 2,
        "norton_ghost_pepper", "Norton Ghost Pepper",
        "Haunted with a benevolent spirit that will cure your ailments.<br>Often found growing in places you didn't ask them to.",
        1 USES, Consumable, LEVEL 2,
        {
            { ConsumableGivesEffect, 0 }
        }
    },
    {
        CT_CONSUMABLE | 3,
        "bleeding_krazaheart", "Bleeding Krazaheart",
        "Don't read too far into it, Kraza is fine.",
        1 USES, Consumable, LEVEL 2,
        {
            { ConsumableEnergyBoost, 30 },
            { ConsumableMoraleBoost, 10 }
        }
    },
    {
        CT_MATERIAL | 0,
        "obsilicon", "Obsilicon",
        "This glassy stone cooled from the same primordial magma<br>that birthed our wafer-thing planes of reality.",
        1 USES, Material, LEVEL 1,
        {
            { MaterialForges, CT_TOOL | 0 }
        }
    },
    {
        CT_MATERIAL | 1,
        "oolite", "Oolite",
        "You've heard small talk from geologists that this may<br>be the first mineral ever constructed.",
        1 USES, Material, LEVEL 1,
        {
            { MaterialForges, CT_TOOL | 1 }
        }
    },
    {
        CT_MATERIAL | 2,
        "cobolt_bar", "Cobolt Bar",
        "Once a foundational metal of the Nachian world, now an outdated curiosity owned mostly by<br>the dwindling, elite of people who still know how to mine it.",
        1 USES, Material, LEVEL 2,
        {
            { MaterialForges, CT_TOOL | 3 }
        }
    },
    {
        CT_MATERIAL | 3,
        "solid_slate", "Solid Slate",
        "A crucial improvement over oldschool spinning schists.",
        1 USES, Material, LEVEL 2,
        {
            { MaterialForges, CT_TOOL | 4 }
        }
    },
    {
        CT_MATERIAL | 4,
        "scandiskium", "Scandiskium",
        "The elemental essence of hard reboots, this metal represents<br> a transitional period in Nachi's geologic history.",
        1 USES, Material, LEVEL 2,
        {
            { MaterialForges, CT_TOOL | 5 }
        }
    },
    {
        CT_TOOL | 0,
        "hello_worldmaker", "Hello Worldmaker",
        "Even the most talented have to start somewhere.",
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
        "The only known attack against shale-256 is the brute force approach.",
        4 USES, ForagingTool, LEVEL 1,
        {
            { ToolEnergyCost, 20 },
            { ToolCanDiscover1, CT_CONSUMABLE | 2 },
            { ToolCanDiscover2, CT_CONSUMABLE | 3 },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 },
        }
    },
    {
        CT_TOOL | 2,
        "basalt_destructor", "Basalt Destructor",
        "It's just plain old dirt - this will do me fine.",
        4 USES, MiningTool, LEVEL 2,
        {
            { ToolEnergyCost, 20 },
            { ToolCanDiscover1, CT_MATERIAL | 2 },
            { ToolCanDiscover2, CT_MATERIAL | 3 },
            { ToolCanDiscover3, CT_MATERIAL | 4 },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 },
            { ToolDiscoverWeight3, 1 },
        }
    },
    {
        CT_TOOL | 5,
        "sepulchre_of_corruption", "Sepulchre of Corruption",
        "Act with care - your own pain will be temporary, but the pain you bring to others, everlasting.",
        2 USES, PrayerTool, LEVEL 2,
        {

        }
    }
};

struct Item {
    ItemCode code {0};
    ItemId id {EMPTY_ID};
    unsigned char uses_left = 0;
    ItemDomain intent = Ordinary;

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const QString &name);

    TooltipText get_tooltip_text() const;
    ItemDefinitionPtr def() const;

    static ItemDefinitionPtr def_of(ItemCode id);
    static ItemDefinitionPtr def_of(const QString &name);
    static ItemDefinitionPtr def_of(const Item &item);
    static QPixmap pixmap_of(ItemCode id);
    static QPixmap pixmap_of(const QString &name);
    static QPixmap pixmap_of(const ItemDefinition &def);
    static QPixmap pixmap_of(const Item &item);
    static ItemId new_instance_id();
    static Item invalid_item();
    static QString type_to_string(ItemType type);

    static Item empty_item;
};
