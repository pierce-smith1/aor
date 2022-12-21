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
using ActivityId = quint64;

const static ItemId EMPTY_ID = 0;
const static ItemId INVALID_ID = 0xffffffffffffffff;
const static ItemCode INVALID_CODE = 0xff;
const static CharacterId NOBODY = 0;
const static ActivityId NO_ACTION = 0;

const static quint64 ACTIONS_TO_HATCH = 25;

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
    Untradeable     = 1 << 14,
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
    ConsumableSpiritBoost,
    ConsumableGivesEffect,
    ConsumableClearsNumEffects,
    ConsumableMakesCouplable,
    PersistentMaxEnergyBoost,
    PersistentMaxSpiritBoost,
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
    HeritageMaxSpiritBoost,
    HeritageConsumableEnergyBoost,
    HeritageSmithProductUsageBoost,
    HeritageInjuryResilience,
    HeritageMaterialValueBonus,
    HeritageActivitySpeedBonus,
    InstanceEggParent1 = 0x1000,
    InstanceEggParent2,
    InstanceEggFoundActionstamp,
    InstanceEggFoundFlavor,
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
    { ToolEnergyCost, "Requires <b>%1 energy</b> per use." },
    { ConsumableEnergyBoost, "Gives <b>+%1 energy</b>." },
    { ConsumableSpiritBoost, "Gives <b>+%1 spirit</b>." },
    { ConsumableClearsNumEffects, "Clears up to <b>%1 effect(s)</b> (starting with the rightmost.)" },
    { ConsumableMakesCouplable, "Gives the ability to <b><font color=purple>have a child</font></b> with another explorer." },
    { PersistentMaxEnergyBoost, "You have <b>+%1 max energy</b>." },
    { PersistentMaxSpiritBoost, "You have <b>+%1 max spirit</b>." },
    { PersistentSpeedBonus, "Your actions complete <b>%1x faster</b>." },
    { PersistentSpeedPenalty, "Your actions complete <b>%1% slower</b>." },
    { PersistentEnergyPenalty, "Your actions cost an additional <b>%1 energy</b>." },
    { HeritageMaxEnergyBoost, "I have <b>+%1 max energy</b>." },
    { HeritageMaxSpiritBoost, "I have <b>+%1 max spirit</b>." },
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
        "<i>It's unusually sticky, but I really like it.</i><br>",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 20 },
            { LeafyResource, 5 },
        }
    },
    {
        CT_CONSUMABLE | 1,
        "byteberry", "Byteberry",
        "<i>These little fruits grow in octuplets on the leaves of tries.</i><br>"
        "<i>Each individual berry is either in a ripe or unripened state, so you have to eat carefully.</i><br>",
        1 USES, Consumable, LEVEL 1,
        {
            { ConsumableEnergyBoost, 10 },
            { ConsumableSpiritBoost, 10 },
            { LeafyResource, 5 },
        }
    },
    {
        CT_MATERIAL | 0,
        "obsilicon", "Obsilicon",
        "<i>I see little flashes of light now and again under its glassy surface -</i><br>"
        "<i>I wonder if it was cooled from the same primordial magma that forged this place.</i><br>",
        1 USES, Material, LEVEL 1,
        {
            { CrystallineResource, 10 },
        }
    },
    {
        CT_MATERIAL | 1,
        "oolite", "Oolite",
        "<i>I've heard small talk that these little egg-shaped stones may</i><br>"
        "<i>have been one the first stones to form in Rhodon.</i><br>"
        "<i>Each one is a little different, but they all fit just as well in my palm.</i><br>",
        1 USES, Material, LEVEL 1,
        {
            { StoneResource, 10 }
        }
    },
    {
        CT_TOOL | 0,
        "maven_mallet", "Maven Mallet",
        "<i>I built this rudimentary tool with the help of a colorful little bird who gave</i><br>"
        "<i>me an annoyingly long-winded series of directions.</i><br>",
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
        "<i>Rhodon is covered in layers of hard Shale-256 - but with a good axe like this,</i><br>"
        "<i>I should be able to brute-force my way through it to find the goodies underneath.</i><br>",
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
        "metamorphic_destructor", "Metamorphic Destructor",
        "<i>It's very important that ore-bearing rocks are destroyed property, or else you can</i><br>"
        "<i>leak their precious minerals back into the earth.</i><br>",
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
        "<i>This pepper grows all over the place, but only very few of them are edible.</i><br>"
        "<i>They've started growing at camp. I ask them to leave but they pretend they're not listening.</i><br>",
        1 USES, Consumable, LEVEL 2,
        {
            { ConsumableClearsNumEffects, 1 }
        }
    },
    {
        CT_CONSUMABLE | 3,
        "bleeding_wildheart", "Bleeding Wildheart",
        "<i>If you're ever climbing a tree and you think you're near the top, just keep going.</i><br>"
        "<i>There are some interesting things up there. I think I found someone's keys.</i><br>",
        1 USES, Consumable, LEVEL 2,
        {
            { ConsumableEnergyBoost, 30 },
            { ConsumableSpiritBoost, 10 }
        }
    },
    {
        CT_CONSUMABLE | 4,
        "pipeapple", "Pipeapple",
        "<i>The pipeapple is a \"fifodesiac\" -</i><br>"
        "<i>That is to say, it allows processes to share information, if you know what I mean.</i><br>",
        1 USES, Consumable, LEVEL 3,
        {
            { LeafyResource, 20 },
            { ConsumableMakesCouplable, 1 }
        }
    },
    {
        CT_MATERIAL | 5,
        "fireclay", "Fireclay",
        "<i>Careful examination of this clay reveals it is selectively permeable -</i><br>"
        "<i>some particles can get through, but others can't.</i><br>"
        "<i>If I put it up to my face, I can't hear anything, but everyone else can still hear me.</i><br>",
        1 USES, Material, LEVEL 2,
        {
            { StoneResource, 50 },
        }
    },
    {
        CT_MATERIAL | 2,
        "cobolt_bar", "Cobolt Bar",
        "<i>Ancient writings reveal that entire cities used to be built of this charming metal,</i><br>"
        "<i>But no one seems to know what it is anymore.</i><br>",
        1 USES, Material, LEVEL 2,
        {
            { MetallicResource, 25 }
        }
    },
    {
        CT_MATERIAL | 3,
        "solid_slate", "Solid Slate",
        "<i>I used to have to carve my notes on these awful spinning schists -</i><br>"
        "<i>powered by rune magic or something else sinister, they were slow, loud, and broke easily.</i><br>"
        "<i>This smaller, more stable stone is a crucial improvement.</i><br>",
        1 USES, Material, LEVEL 2,
        {
            { StoneResource, 5 },
            { RunicResource, 10 }
        }
    },
    {
        CT_MATERIAL | 4,
        "scandiskium", "Scandiskium",
        "<i>Every once in a while, stillness grips the whole of Rhodon as it falls into night.</i><br>"
        "<i>Sometimes this happens very suddenly. I get awful headaches and nausea from it.</i><br>"
        "<i>And I can hear things changing as the world re-roots itself...</i><br>",
        1 USES, Material, LEVEL 2,
        {
            { CrystallineResource, 10 },
            { MetallicResource, 5 },
        }
    },
    {
        CT_TOOL | 3,
        "seaquake", "Seaquake",
        "<i>It makes a very satisfying clang.</i><br>",
        4 USES, SmithingTool, LEVEL 3,
        {
            { ToolEnergyCost, 30 },
            { CostMetallic, 10 },
            { CostCrystalline, 30 },
            { ToolMaximumMetallic, 80 },
            { ToolMaximumRunic, 20 },
        }
    },
    {
        CT_TOOL | 4,
        "defragmenter", "Defragmenter",
        "<i>You very rarely see flat, even bands of rock in Rhodon -</i><br>"
        "<i>as the world fills up, it becomes harder and harder to lay the sediment down sequentially.</i><br>",
        1 USES, MiningTool, LEVEL 3,
        {
            { ToolEnergyCost, 30 },
            { CostMetallic, 40 },
            { CostRuinc, 15 },
            { ToolCanDiscover1, CT_ARTIFACT | 0 },
            { ToolCanDiscover2, CT_ARTIFACT | 1 },
            { ToolCanDiscover3, CT_MATERIAL | 5 },
            { ToolDiscoverWeight1, 1 },
            { ToolDiscoverWeight2, 1 },
            { ToolDiscoverWeight3, 1 },
        }
    },
    {
        CT_ARTIFACT | 0,
        "recovered_journal", "Recovered Journal",
        "<i>It's in an odd language I can't quite read -</i><br>"
        "<i>but I can gather it tells a sad story of an orphan far from /home.<br><br>",
        0 USES, Artifact, LEVEL 3,
        {
            { PersistentMaxSpiritBoost, 40 }
        }
    },
    {
        CT_ARTIFACT | 1,
        "scalped_remains", "Scalped Remains",
        "<i>Perhaps a marker of a previous failed expedition, this is definitely a Fennahian skull.</i><br>"
        "<i>However, it remains animated by her soul, supposedly because the spirits of Rhodon haven't</i><br>"
        "<i>figured out what to do with her yet.</i><br>"
        "<i>In the meantime, I'll keep her around, as I am eager for company.</i><br>",
        0 USES, Artifact, LEVEL 3,
        {
            { PersistentMaxEnergyBoost, 40 }
        }
    },
    {
        CT_EFFECT | 0,
        "disconnected_socket", "Disconnected Socket",
        "<i>I took quite a nasty fall and dislocated my shoulder.</i><br>"
        "<i>The doctor said doing a handshake would reconnect it, but I keep being refused.</i><br>",
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
        "<i>I've gone without food for too long -</i><br>"
        "<i>sometimes I am reminded of spaghetti nights back home, where there weren't enough</i><br>"
        "<i>forks to go around and you had to just sit and think about how hungry you were while</i><br>"
        "<i>everyone else ate.</i><br>",
        3 USES, Effect, LEVEL 1,
        {
            { PersistentSpeedPenalty, 20 },
        }
    },
    {
        CT_EFFECT | 2,
        "weakness", "Weakness",
        "<i>There's a numbness to my whole body -</i><br>"
        "<i>I feel like my soul no longer owns it, and so I'm just watching myself stumble around.</i><br>",
        3 USES, Effect, LEVEL 1,
        {
            { PersistentEnergyPenalty, 10 },
        }
    },
    {
        CT_OTHER | 0,
        "fennahian_egg", "Fennahian Egg",
        "<i>I could make an omelette out of it, but maybe it's best to just wait for it to hatch.</i><br>",
        1 USES, Consumable | Untradeable, LEVEL 1,
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
    ActivityId owning_action = NO_ACTION;
    ItemProperties instance_properties = {};

    Item() = default;
    explicit Item(const ItemDefinition &def);
    explicit Item(ItemDefinitionPtr def);
    explicit Item(ItemCode id);
    explicit Item(const QString &name);

    static Item make_egg(CharacterId parent1, CharacterId parent2);
    static Item make_egg();

    ItemDefinitionPtr def() const;
    QString instance_properties_to_string() const;

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
