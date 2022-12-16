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
#define LEVEL

const static int SMITHING_SLOTS = 5;
const static int SMITHING_SLOTS_PER_ROW = 5;

const static int TRADE_SLOTS = 5;

const static int ARTIFACT_SLOTS = 3;

const static int EFFECT_SLOTS = 4;

using ItemCode = quint16;
using ItemId = quint64;
using CharacterId = quint16;
using GameId = quint64;

const static ItemId EMPTY_ID = 0;
const static ItemId INVALID_ID = 0xffffffffffffffff;
const static ItemCode INVALID_CODE = 0xff;
const static CharacterId NOBODY = 0xffff;

const static quint64 ACTIONS_TO_HATCH = 2;

using ItemType = quint16;
enum ItemDomain : ItemType {
    Ordinary        = 0,      None = 0,
    Consumable      = 1 << 0, Eating   = 1 << 0,
    Material        = 1 << 1,
    SmithingTool    = 1 << 2, Smithing = 1 << 2,
    ForagingTool    = 1 << 3, Foraging = 1 << 3,
    MiningTool      = 1 << 4, Mining   = 1 << 4,
    Offering        = 1 << 5, Trading  = 1 << 5,
    Blessing        = 1 << 6,
    Artifact        = 1 << 7,
    Effect          = 1 << 8,
    Rune            = 1 << 9,
    Portrait        = 1 << 10,
    ForeignOffering = 1 << 11,
    Defiling        = 1 << 12,
    Coupling        = 1 << 13, Explorer = 1 << 13, Egg = 1 << 13,
    Tool            = SmithingTool | ForagingTool | MiningTool
};

const static int CT_EMPTY = 0;
const static int CT_CONSUMABLE = 1 << 8;
const static int CT_MATERIAL = 1 << 9;
const static int CT_TOOL = 1 << 10;
const static int CT_ARTIFACT = 1 << 11;
const static int CT_EFFECT = 1 << 12;
const static int CT_RUNE = 1 << 13;
const static int CT_OTHER = 1 << 14;

enum ItemProperty : quint16 {
    ToolEnergyCost,
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
    ConsumableEnergyBoost,
    ConsumableMoraleBoost,
    ConsumableGivesEffect,
    ConsumableClearsNumEffects,
    ConsumableMakesCouplable,
    PersistentMaxEnergyBoost,
    PersistentMaxMoraleBoost,
    PersistentSpeedBonus,
    PersistentSpeedPenalty,
    PersistentEnergyPenalty,
    InjurySmithing,
    InjuryForaging,
    InjuryMining,
    InjuryTradinge,
    InjuryEating,
    InjuryDefiling,
    InjuryTrading,
    InjuryCoupling, // COCK INJURED
    HeritageMaxEnergyBoost,
    HeritageMaxMoraleBoost,
    HeritageConsumableEnergyBoost,
    HeritageSmithProductUsageBoost,
    HeritageInjuryResilience,
    HeritageMaterialValueBonus,
    HeritageActivitySpeedBonus,
    InstanceEggParent1,
    InstanceEggParent2,
    InstanceEggFoundActionstamp,
    Cost = 0x2000,
    CostStone = 0x2001,
    CostMetallic = 0x2002,
    CostCrystalline = 0x2003,
    CostRuinc = 0x2004,
    CostLeafy = 0x2005,
    ToolMaximum = 0x4000,
    ToolMaximumStone = 0x4001,
    ToolMaximumMetallic = 0x4002,
    ToolMaximumCrystalline = 0x4003,
    ToolMaximumRunic = 0x4004,
    ToolMaximumLeafy = 0x4005,
    Resource = 0x8000,
    StoneResource = 0x8001,
    MetallicResource = 0x8002,
    CrystallineResource = 0x8003,
    RunicResource = 0x8004,
    LeafyResource = 0x8005,
};

const static std::map<ItemProperty, QString> PROPERTY_DESCRIPTIONS = {
    { ToolEnergyCost, "Costs <b>%1 energy</b> per use." },
    { ConsumableEnergyBoost, "Gives <b>+%1 energy</b>." },
    { ConsumableMoraleBoost, "Gives <b>+%1 spirit</b>." },
    { ConsumableClearsNumEffects, "Clears up to <b>%1 effect(s)</b> (starting with the rightmost.)" },
    { ConsumableMakesCouplable, "Gives the ability to <b><font color=purple>have a child</font></b> with another explorer." },
    { PersistentMaxEnergyBoost, "You have <b>+%1 max energy</b>." },
    { PersistentMaxMoraleBoost, "You have <b>+%1 max spirit</b>." },
    { PersistentSpeedBonus, "Your actions complete <b>%1x faster</b>." },
    { PersistentSpeedPenalty, "Your actions complete <b>%1% slower</b>." },
    { PersistentEnergyPenalty, "Your actions cost an additional <b>%1 energy</b>." },
    { HeritageMaxEnergyBoost, "I have <b>+%1 max energy</b>." },
    { HeritageMaxMoraleBoost, "I have <b>+%1 max morale</b>." },
    { HeritageConsumableEnergyBoost, "I get <b>+%1 bonus energy</b> when I eat something." },
    { HeritageSmithProductUsageBoost, "Items that I craft have <b>+%1 use(s)</b>." },
    { HeritageInjuryResilience, "I have a <b>-%1% chance to suffer an injury</b> after taking an action." },
    { HeritageMaterialValueBonus, "Materials are <b>worth %1% more</b> when I use them." },
    { HeritageActivitySpeedBonus, "My actions take <b>%1% less time</b>." },
};

// This basically just wraps a std::map<ItemPropety, int>,
// with the crucial change that operator[] is const while retaining the
// behavior of returning zero-initialized values for non-existant keys.
class ItemProperties {
public:
    ItemProperties() = default;
    ItemProperties(std::initializer_list<std::pair<const ItemProperty, quint16>> map);
    quint16 operator[](ItemProperty prop) const;
    std::map<ItemProperty, quint16>::const_iterator begin() const;
    std::map<ItemProperty, quint16>::const_iterator end() const;

    std::map<ItemProperty, quint16> map;
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
        "Empty slot.",
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
            { LeafyResource, 5 },
        }
    },
    {
        CT_CONSUMABLE | 1,
        "byteberry", "Byteberry",
        "<i>Grown on the leaves of tries, these make an excellent treat when lightly charred.</i>",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 10 },
            { ConsumableMoraleBoost, 10 },
            { LeafyResource, 5 },
        }
    },
    {
        CT_MATERIAL | 0,
        "obsilicon", "Obsilicon",
        "<i>This glassy stone cooled from the same primordial magma<br>"
        "that birthed Rhodon's wafer-thin planes of reality.</i>",
        1 USES, Material, LEVEL 1,
        {
            { CrystallineResource, 10 },
        }
    },
    {
        CT_MATERIAL | 1,
        "oolite", "Oolite",
        "<i>You've heard small talk from geologists that this may<br>"
        "have been the first rock ever constructed by the earth.</i>",
        1 USES, Material, LEVEL 1,
        {
            { StoneResource, 10 }
        }
    },
    {
        CT_TOOL | 0,
        "maven_mallet", "Maven Mallet",
        "<i>A rudimentary tool. You've named it after the maven, a strange<br>"
        "little bird you've seen around here that tells you long-winded stories.</i>",
        3 USES, SmithingTool, LEVEL 1,
        {
            { CostStone, 10 },
            { CostCrystalline, 10 },
            { ToolEnergyCost, 20 },
            { ToolMaximumStone, 30 },
            { ToolMaximumCrystalline, 30 },
        }
    },
    {
        CT_TOOL | 1,
        "hashcracker", "Hashcracker",
        "<i>Rhodon is covered in layers of hard shale-256 - but with a good axe like this,<br>"
        "you should be able to brute-force your way through it to find the goodies underneath.</i>",
        4 USES, ForagingTool, LEVEL 1,
        {
            { CostStone, 30 },
            { CostCrystalline, 10 },
            { ToolEnergyCost, 10 },
            { ToolCanDiscover1, CT_CONSUMABLE | 2 },
            { ToolCanDiscover2, CT_CONSUMABLE | 3 },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 },
        }
    },
    {
        CT_TOOL | 2,
        "basalt_destructor", "Basalt Destructor",
        "<i>It's just plain old dirt - this will be good enough.</i>",
        4 USES, MiningTool, LEVEL 2,
        {
            { CostStone, 10 },
            { CostCrystalline, 30 },
            { ToolEnergyCost, 10 },
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
        "<i>Don't read too far into it, Kraza is fine.</i>",
        1 USES, Consumable, LEVEL 2,
        {
            { ConsumableEnergyBoost, 30 },
            { ConsumableMoraleBoost, 10 }
        }
    },
    {
        CT_CONSUMABLE | 4,
        "pipeapple", "Pipeapple",
        "<i>This fruit acts as a powerful fifodesiac, making it irresistable<br>"
        "to share your genetic data with someone else.</i>",
        1 USES, Consumable, LEVEL 3,
        {
            { ConsumableMakesCouplable, 1 }
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
        }
    },
    {
        CT_MATERIAL | 3,
        "solid_slate", "Solid Slate",
        "<i>A crucial improvement over oldschool spinning schists.</i>",
        1 USES, Material, LEVEL 2,
        {
        }
    },
    {
        CT_MATERIAL | 4,
        "scandiskium", "Scandiskium",
        "<i>Whenever Rhodon is torn into darkness by a Great Reroot,<br>"
        "another vein of this soft, yellowish metal is born.</i>",
        1 USES, Material, LEVEL 2,
        {
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
        }
    },
    {
        CT_TOOL | 4,
        "disk_fragmenter", "Disk Fragmenter",
        "This is how it happens.",
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
        CT_ARTIFACT | 0,
        "recovered_journal", "Recovered Journal",
        "<i>Tells a sad story of an orphan far from home.</i>",
        0 USES, Artifact, LEVEL 3,
        {
            { PersistentMaxMoraleBoost, 20 }
        }
    },
    {
        CT_ARTIFACT | 1,
        "scalped_remains", "Scalped Remains",
        "<i>The operating spirits of Rhodon haven't decided what to do<br>"
        "with this unfortunate Fennahian's soul yet, so her ghost still"
        "inhabits her decapitated skull. She gives good advice</i>",
        0 USES, Artifact, LEVEL 3,
        {
            { PersistentMaxEnergyBoost, 20 }
        }
    },
    {
        CT_EFFECT | 0,
        "disconnected_socket", "Disconnected Socket",
        "<i>You had a nasty fall and dislocated your shoulder.<br>"
        "Your body has sent an RST blood cell to your arm to notify it<br>"
        "that connection has been lost.</i>",
        5 USES, Effect, LEVEL 1,
        {
            { PersistentSpeedPenalty, 20 },
            { InjuryMining, 1 },
            { InjuryForaging, 1 },
            { InjuryTrading, 1 },
        }
    },
    {
        CT_EFFECT | 1,
        "starving", "Starving",
        "<i>This is what happens when the jackass philosopher in the<br>"
        "seat next to you won't put down their spaghetti forks.</i><br>",
        3 USES, Effect, LEVEL 1,
        {
            { PersistentSpeedPenalty, 20 },
        }
    },
    {
        CT_EFFECT | 2,
        "weakness", "Weakness",
        "<i>All these things I thought I owned are falling between my fingers...</i>",
        3 USES, Effect, LEVEL 1,
        {
            { PersistentEnergyPenalty, 10 },
        }
    },
    {
        CT_OTHER | 0,
        "fennahian_egg", "Fennahian Egg",
        "<i>You could make an omelette out of it, but maybe it's<br>"
        "best to just wait for it to hatch...</i>",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 50 }
        }
    }
};

struct Item {
    ItemCode code = 0;
    ItemId id = EMPTY_ID;
    unsigned char uses_left = 0;
    ItemDomain intent = Ordinary;
    CharacterId intent_holder = NOBODY;
    ItemProperties instance_properties = {};

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const QString &name);

    static Item make_egg(CharacterId parent1, CharacterId parent2);
    static Item make_egg();

    ItemDefinitionPtr def() const;

    static ItemDefinitionPtr def_of(ItemCode id);
    static ItemDefinitionPtr def_of(const QString &name);
    static ItemDefinitionPtr def_of(const Item &item);
    static ItemCode code_of(const QString &name);
    static QPixmap pixmap_of(ItemCode id);
    static QPixmap pixmap_of(const QString &name);
    static QPixmap pixmap_of(const ItemDefinition &def);
    static QPixmap pixmap_of(const Item &item);
    static QPixmap sil_pixmap_of(ItemCode id);
    static Item invalid_item();
    static bool has_resource_value(ItemCode code);

    static void for_each_resource_type(const std::function<void(ItemProperty, ItemProperty, ItemProperty)> &fn);
    static void for_each_tool_discover(const std::function<void(ItemProperty, ItemProperty)> &fn);

    static QString type_to_string(ItemType type);
    static QString properties_to_string(const ItemProperties &props);

    static Item empty_item;
};
