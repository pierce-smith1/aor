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

#define USES
#define NOT_CRAFTABLE {}
#define NOT_TOOL {}

const static int INVENTORY_ROWS = 4;
const static int INVENTORY_COLS = 7;
const static int INVENTORY_SIZE = INVENTORY_COLS * INVENTORY_ROWS;

using ItemCode = std::uint16_t;
using ItemId = std::uint64_t;
using ForeignItemId = std::uint64_t;

const static ItemId EMPTY_ID = 0;
const static ItemId INVALID_ID = 0xffffffffffffffff;
const static ItemCode INVALID_CODE = 0xff;

enum ItemType {
    NoType,
    Consumable,
    Material,
    ForagingTool,
    SmithingTool,
    TradingTool,
    PrayerTool,
    Artifact,
};

const static int FTOOL_FORAGING_TREE = 1 << 0;
const static int FTOOL_FORAGING_MUSHROOM = 1 << 1;
const static int FTOOL_FORAGING_INSECT = 1 << 2;
const static int FTOOL_FORAGING_RIVER = 1 << 3;
const static int FTOOL_MINING_METAL = 1 << 4;
const static int FTOOL_MINING_CRYSTAL = 1 << 5;
const static int FTOOL_MINING_ARTIFACT = 1 << 6;
const static int FTOOL_PRAYER = 1 << 7;
const static int FTOOL_ANY = 0x7f;

enum ItemProperty {
    EnergyBoost,
    MoraleBoost,
    MaterialPower,
    MaterialFlags,
    EnergyCost,
    ToolPower,
    ToolFlags,
    PrayerPower,
};

// This basically just wraps a std::map<ItemPropety, int>,
// with the crucial change that operator[] is const while retaining the
// behavior of returning zero-initialized values for non-existant keys.
class ItemProperties {
public:
    ItemProperties(std::initializer_list<std::pair<const ItemProperty, int>> map);
    int operator[](ItemProperty prop) const;

private:
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
    ItemNameList recipe;
    ItemChances result_chances;
    ItemProperties properties;
};

using ItemDefinitionPtr = std::vector<ItemDefinition>::const_iterator;

const static std::vector<ItemDefinition> ITEM_DEFINITIONS = {
    {
        __COUNTER__,
        "empty",
        "Empty",
        "Nothing here.",
        0 USES,
        NoType,
        NOT_CRAFTABLE,
        NOT_TOOL,
        {}
    },
    {
        __COUNTER__,
        "globfruit",
        "Globfruit",
        "A cluster of wild starfruit.",
        1 USES,
        Consumable,
        NOT_CRAFTABLE,
        NOT_TOOL,
        {
            { EnergyBoost, 20 },
            { MoraleBoost, 20 }
        }
    },
    {
        __COUNTER__,
        "eliding_hatchet",
        "Eliding Hatchet",
        "Cast away the bark from the tree, and you'll uncover all sorts of bugs!",
        3 USES,
        ForagingTool,
        {},
        {},
        {
            { ToolFlags, FTOOL_FORAGING_INSECT },
            { EnergyCost, 40 },
            { ToolPower, 2 },
        }
    },
    {
        __COUNTER__,
        "root_kit",
        "Root kit",
        "Not very tasty, but you have to eat something.",
        3 USES,
        Consumable,
        NOT_CRAFTABLE,
        NOT_TOOL,
        {
            { EnergyBoost, 10 },
        }
    },
    {
        __COUNTER__,
        "silicon_bar",
        "Silicon Bar",
        "The stuff electric dreams are made of.",
        1 USES,
        Material,
        NOT_CRAFTABLE,
        NOT_TOOL,
        {
            { MaterialFlags, FTOOL_MINING_METAL },
            { MaterialPower, 1 }
        }
    }
};

enum ItemIntent : unsigned char {
    None,
    ToUse,          // marked for smithing, praying, eating, etc.
    ToSell,         // to be sold in a trade
    ToPurchaseWith, // to be used for buying another player's trade
    ToRecieve,      // not currently in my inventory; to be gained in a trade
};

struct Item {
    ItemCode code;
    ItemId id;
    unsigned char uses_left;
    ItemIntent intent;

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const std::string &name);

    static ItemDefinitionPtr def_of(ItemCode id);
    static ItemDefinitionPtr def_of(const std::string &name);
    static ItemDefinitionPtr def_of(const Item &item);
    static QPixmap pixmap_of(ItemCode id);
    static QPixmap pixmap_of(const std::string &name);
    static QPixmap pixmap_of(const ItemDefinition &def);
    static ItemId new_instance_id();
    static Item invalid_item();
};
