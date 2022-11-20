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
#define NOT_CRAFTABLE {}
#define NOT_TOOL {}

const static int INVENTORY_ROWS = 4;
const static int INVENTORY_COLS = 7;
const static int INVENTORY_SIZE = INVENTORY_COLS * INVENTORY_ROWS;

const static int SMITHING_SLOTS = 6;
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

using ItemType = int;

const static int IT_NONE = 0;
const static int IT_CONSUMABLE = 1 << 0;
const static int IT_MATERIAL = 1 << 1;
const static int IT_FORAGING_TOOL = 1 << 2;
const static int IT_MINING_TOOL = 1 << 3;
const static int IT_PRAYER_TOOL = 1 << 4;
const static int IT_BLESSING = 1 << 5;
const static int IT_ARTIFACT = 1 << 6;
const static int IT_EFFECT = 1 << 7;
const static int IT_RUNE = 1 << 8;
const static int IT_MAX_FLAG = 1 << 20;
const static int IT_TOOL = IT_FORAGING_TOOL | IT_MINING_TOOL | IT_PRAYER_TOOL;

const static int CT_EMPTY = 0;
const static int CT_CONSUMABLE = 1 << 8;
const static int CT_MATERIAL = 1 << 9;
const static int CT_TOOL = 1 << 10;
const static int CT_ARTIFACT = 1 << 11;
const static int CT_EFFECT = 1 << 12;
const static int CT_RUNE = 1 << 13;
const static int CT_OTHER = 1 << 14;

enum ItemProperty {
    EnergyBoost,
    MoraleBoost,
    EnergyCost,
    MoraleCost,
    GivesEffectOnConsume,
};

// This basically just wraps a std::map<ItemPropety, int>,
// with the crucial change that operator[] is const while retaining the
// behavior of returning zero-initialized values for non-existant keys.
class ItemProperties {
public:
    ItemProperties(std::initializer_list<std::pair<const ItemProperty, int>> map);
    int operator[](ItemProperty prop) const;

    std::map<ItemProperty, int> map;
};

using ItemNameList = std::vector<std::string>;
using ItemChances = std::set<std::pair<std::string, double>>;

struct ItemDefinition {
    ItemCode code;
    std::string internal_name;
    std::string display_name;
    std::string description;
    unsigned char default_uses_left;
    ItemType type;
    ItemProperties properties;
};

using ItemDefinitionPtr = std::vector<ItemDefinition>::const_iterator;

const static std::vector<ItemDefinition> ITEM_DEFINITIONS = {
    {
        CT_EMPTY,
        "empty", "Empty",
        "Nothing here.",
        0 USES, IT_NONE,
        {}
    },
    {
        CT_CONSUMABLE | 0,
        "globfruit", "Globfruit",
        "A relative of the starfruit, this one is a lot stickier.",
        1 USES, IT_CONSUMABLE,
        {
            { EnergyBoost, 20 },
        }
    },
    {
        CT_CONSUMABLE | 1,
        "silicon_wafer", "Silicon Wafer",
        "Melts down in your mouth!",
        1 USES, IT_CONSUMABLE | IT_MATERIAL,
        {
            { EnergyBoost, 30 },
            { MoraleBoost, 30 }
        }
    },
    {
        CT_MATERIAL | 0,
        "data_leaf", "Data Leaf",
        "Gently fallen from a red-black tree.",
        1 USES, IT_MATERIAL,
        {}
    },
    {
        CT_MATERIAL | 1,
        "bark_cylinder", "Bark Cylinder",
        "When you crack open a hard drive, you can tell how old it is by looking at the rings!",
        1 USES, IT_MATERIAL,
        {}
    },
    {
        CT_MATERIAL | 2,
        "rusted_bar", "Rusted Bar",
        "Go ahead and borrow it, I'm sure nobody will mind.",
        1 USES, IT_MATERIAL,
        {}
    },
    {
        CT_TOOL | 0,
        "eliding_hatchet", "Eliding Hatchet",
        "They say the bark off some trees has mystical properties, but start stripping it<br>away and you'll realize underneath every magical tree is just a regular one.",
        3 USES, IT_FORAGING_TOOL,
        {
            { EnergyCost, 20 }
        }
    },
    {
        CT_TOOL | 1,
        "spearfisher", "Spearfisher",
        "I know a friend of yours who will love this.",
        3 USES, IT_FORAGING_TOOL,
        {
            { EnergyCost, 30 }
        }
    },
    {
        CT_ARTIFACT | 0,
        "dynamic_backpack", "Dynamic Backpack",
        "When you encounter another player for trade, you both have an <b>extra slot</b> to use.",
        0 USES, IT_ARTIFACT,
        {}
    }
};

enum ItemIntent : unsigned char {
    NoIntent,
    ToBeMaterial,
    ToBeOffered,
    ToBeEaten,
    UsingAsTool,
    UsingAsArtifact,
    IsEffect,
};

struct Item {
    ItemCode code = 0;
    ItemId id = EMPTY_ID;
    unsigned char uses_left = 0;
    ItemIntent intent = NoIntent;

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const std::string &name);

    TooltipText get_tooltip_text();
    ItemDefinitionPtr def();

    static ItemDefinitionPtr def_of(ItemCode id);
    static ItemDefinitionPtr def_of(const std::string &name);
    static ItemDefinitionPtr def_of(const Item &item);
    static QPixmap pixmap_of(ItemCode id);
    static QPixmap pixmap_of(const std::string &name);
    static QPixmap pixmap_of(const ItemDefinition &def);
    static QPixmap pixmap_of(const Item &item);
    static ItemId new_instance_id();
    static Item invalid_item();
    static std::string type_to_string(ItemType type);
};
