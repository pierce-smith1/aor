#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

#include <QtGlobal>
#include <QPixmap>

const static int IF_NOTHING_SPECIAL   = 0;
const static int IF_UNTRADEABLE       = 1 << 0;
const static int IF_TOOL              = 1 << 1;
const static int IF_CONSUMABLE        = 1 << 2;
const static int IF_SMITHING_MATERIAL = 1 << 3;
const static int IF_PRAYER_ITEM       = 1 << 4;
const static int IF_DIVINE_ITEM       = 1 << 5;
const static int IF_ANIMAL            = 1 << 6;

const static int INVENTORY_ROWS = 4;
const static int INVENTORY_COLS = 5;
const static int INVENTORY_SIZE = INVENTORY_COLS * INVENTORY_ROWS;

using ItemCode = std::uint16_t;
using ItemId = std::uint64_t;
using ForeignItemId = std::uint64_t;

const static ItemId EMPTY_ID = 0;

struct ItemDefinition {
    std::string internal_name;
    std::string display_name;
    std::string description;
    unsigned char default_uses_left;
    unsigned char flags;
    ItemCode code;
};

using ItemDefinitionPtr = std::vector<ItemDefinition>::const_iterator;

// IMPORTANT: The item id of whatever item you define MUST match its
// index in this vector!
const static std::vector<ItemDefinition> ITEM_DEFINITIONS = {
    {
        "empty",
        "Empty",
        "Nothing here.",
        0,
        IF_NOTHING_SPECIAL,
        0
    },
    {
        "globfruit",
        "Globfruit",
        "A cluster of wild starfruit.",
        1,
        IF_CONSUMABLE,
        1,
    },
    {
        "eliding_hatchet",
        "Eliding Hatchet",
        "You never know what you'll find beneath a tree's bark.",
        3,
        IF_TOOL,
        2,
    },
    {
        "root_kit",
        "Root kit",
        "A box of neat roots you found.",
        3,
        IF_CONSUMABLE,
        3,
    },
};

enum ItemIntent {
    None,
    ToUse,          // marked for smithing, praying, eating, etc.
    ToSell,         // to be sold in a trade
    ToPurchaseWith, // to be used for buying another player's trade
    ToRecieve,      // not currently in my inventory; to be gained in a trade
};

struct Item {
    ItemCode code = 0;
    ItemId id = EMPTY_ID;
    unsigned char uses_left = 0;
    ItemIntent intent = ItemIntent::None;

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const std::string &name);

    static ItemDefinitionPtr def_of(ItemCode id);
    static ItemDefinitionPtr def_of(const std::string &name);
    static QPixmap pixmap_of(ItemCode id);
    static QPixmap pixmap_of(const std::string &name);
    static QPixmap pixmap_of(const ItemDefinition &def);
    static ItemId new_instance_id();
};


