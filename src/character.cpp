#include "character.h"
#include "game.h"
#include "gamewindow.h"

Character::Character()
    : m_activity(NOBODY, None) { }

Character::Character(CharacterId id, const QString &name)
    : m_id(id),
      m_name(name),
      m_color(Generators::color()),
      m_activity(m_id, None) { }

QString &Character::name() {
    return m_name;
}

Color &Character::color() {
    return m_color;
}

CharacterActivity &Character::activity() {
    return m_activity;
}

CharacterId Character::id() {
    return m_id;
}

void Character::start_activity(ItemDomain domain) {
    if (domain == None) {
        m_activity = CharacterActivity(m_id, None);
        return;
    }

    qint64 activity_ms = 1000 * 120;
    if (m_tool_ids.find(domain) != end(m_tool_ids) && m_tool_ids.at(domain) != EMPTY_ID) {
        activity_ms *= gw()->game().inventory().get_item(m_tool_ids.at(domain)).def()->item_level;
    }

    m_activity = CharacterActivity(m_id, domain, activity_ms, activity_ms);
    gw()->refresh_ui_buttons();
}

quint16 &Character::energy() {
    if (m_energy > max_energy()) {
        m_energy = max_energy();
    }

    return m_energy;
}

quint16 &Character::morale() {
    if (m_morale > max_morale()) {
        m_morale = max_morale();
    }

    return m_morale;
}

int Character::max_energy() {
    const auto &artifacts = external_items().at(Artifact);

    return BASE_MAX_ENERGY + std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + gw()->game().inventory().get_item(b).def()->properties[ArtifactMaxEnergyBoost];
    });
}

int Character::max_morale() {
    const auto &artifacts = external_items().at(Artifact);

    return BASE_MAX_ENERGY + std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + gw()->game().inventory().get_item(b).def()->properties[ArtifactMaxMoraleBoost];
    });
}

void Character::add_energy(int add) {
    if (-add > m_energy) {
        m_energy = 0;
        return;
    }

    m_energy += add;
    if (m_energy > max_energy()) {
        m_energy = max_energy();
    }
}

void Character::add_morale(int add) {
    if (-add > m_morale) {
        m_morale = 0;
        return;
    }

    m_morale += add;
    if (m_morale > max_morale()) {
        m_morale = max_morale();
    }
}

bool Character::can_perform_action(ItemDomain domain) {
    switch (domain) {
        case Eating: {
            return !m_activity.ongoing();
        }
        case Smithing: {
            const auto &materials = external_items()[Material];
            bool enough_materials = std::any_of(begin(materials), begin(materials) + SMITHING_SLOTS, [&](ItemId a) {
                return a != EMPTY_ID;
            });

            return enough_materials && !m_activity.ongoing();
        }
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id(domain));
            return energy() >= tool.def()->properties[ToolEnergyCost] && !m_activity.ongoing();
        }
        default: {
            qFatal("Tried to assess whether character (%s) can do invalid action domain (%d)", m_name.toStdString().c_str(), domain);
        }
    }
}

int Character::energy_to_gain() {
    int gain;

    switch (m_activity.action()) {
        case Eating: {
            std::vector<Item> inputs = gw()->game().inventory().items_of_intent(m_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableEnergyBoost];
            });
            break;
        }
        case Smithing:
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id());
            gain = tool.def()->properties[ToolEnergyCost];
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    if (morale() > (double) max_morale() * 0.5) {
        if (gain < 0) {
            gain /= 2;
        }
    }

    return gain;
}

int Character::morale_to_gain() {
    int gain;

    switch (m_activity.action()) {
        case Eating: {
            std::vector<Item> inputs = gw()->game().inventory().items_of_intent(m_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableMoraleBoost];
            });
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    return gain;
}

std::vector<Item> Character::input_items() {
    switch (m_activity.action()) {
        case Eating: {
            return gw()->game().inventory().items_of_intent(m_id, Eating);
        }
        case Smithing: {
            return gw()->game().inventory().items_of_intent(m_id, Material);
        }
        case Trading: {
            return gw()->game().inventory().items_of_intent(m_id, Offering);
        }
        default: {
            return {};
        }
    }
}

std::vector<Item> Character::generate_output_items() {
    std::vector<Item> outputs;

    Item tool = gw()->game().inventory().get_item(tool_id(m_activity.action()));
    const ItemProperties &tool_properties = tool.def()->properties;

    switch (m_activity.action()) {
        case Smithing: {
            break;
        }
        case Foraging:
        case Mining: {
            if (tool.id == EMPTY_ID) {
                if (m_activity.action() == Foraging) {
                    return {
                        Generators::sample_with_weights<Item>({
                            { Item("globfruit"), 1 },
                            { Item("byteberry"), 1 }
                        })
                    };
                } else {
                    return {
                        Generators::sample_with_weights<Item>({
                            { Item("obsilicon"), 1 },
                            { Item("oolite"), 1 }
                        })
                    };
                }
            }

            std::vector<std::pair<Item, double>> possible_discoveries;
            for (int i = (int) ToolCanDiscover1; i <= (int) ToolCanDiscover9; i++) {
                if (tool_properties[(ItemProperty) i] != 0) {
                    possible_discoveries.emplace_back(
                        tool_properties[(ItemProperty) i],
                        tool_properties[(ItemProperty) (i + 9)]
                    );
                }
            }

            outputs.emplace_back(Generators::sample_with_weights(possible_discoveries));

            break;
        }
        case Trading: {
            // Trading generates no base items; must be handled elsewhere
            return {};
        }
        default: {
            qWarning("Tried to generate items with unknown action domain (%d)", m_activity.action());
        }
    }

    return outputs;
}

std::vector<std::pair<ItemCode, bool>> Character::smithable_items() {
    ItemDefinitionPtr smithing_def = gw()->game().inventory().get_item(tool_id(SmithingTool)).def();

    std::vector<std::pair<ItemCode, bool>> smithable_codes;

    for (const ItemDefinition &def : ITEM_DEFINITIONS) {
        bool tool_is_sufficient = true;
        bool is_smithable = false;

        for (quint16 i = 1; i <= 5; i++) {
            ItemProperty resource_cost = (ItemProperty) (Cost + i);
            ItemProperty resource_max = (ItemProperty) (ToolMaximum + i);

            // If at least one cost is non-zero, the item is smithable
            if (def.properties[resource_cost] > 0) {
                is_smithable = true;
            }

            if (smithing_def->properties[resource_max] < def.properties[resource_cost]) {
                tool_is_sufficient = false;
                break;
            }
        }

        if (tool_is_sufficient && is_smithable) {
            bool can_smith = true;

            for (quint16 i = 1; i <= 5; i++) {
                ItemProperty resource_cost = (ItemProperty) (Cost + i);
                ItemProperty resource = (ItemProperty) (Resource + i);

                int resource_budget = std::accumulate(
                    begin(external_items().at(Material)),
                    end(external_items().at(Material)),
                    0,
                    [=](ItemId a, int b) {
                        return gw()->game().inventory().get_item(a).def()->properties[resource] + b;
                    }
                );

                if (resource_budget < def.properties[resource_cost]) {
                    can_smith = false;
                    break;
                }
            }

            smithable_codes.push_back(std::make_pair(def.code, can_smith));
        }
    }

    return smithable_codes;
}

bool Character::push_effect(const Item &effect) {
    if (effect.id == EMPTY_ID) {
        return false;
    }

    for (int i = 0; i < EFFECT_SLOTS; i++) {
        if (effects()[i].id == EMPTY_ID) {
            effects()[i] = effect;
            return true;
        }
    }

    return false;
}

bool Character::clear_last_effect() {
    for (int i = EFFECT_SLOTS - 1; i >= 0; i--) {
        if (effects()[i].id != EMPTY_ID) {
            effects()[i] = Item();
            return true;
        }
    }

    return false;
}

ItemId Character::tool_id() {
    return m_tool_ids[m_activity.action()];
}

ItemId Character::tool_id(ItemDomain domain) {
    return m_tool_ids[domain];
}

ToolIds &Character::tools() {
    return m_tool_ids;
}

ExternalItemIds &Character::external_items() {
    return m_external_item_ids;
}

Effects &Character::effects() {
    return m_effects;
}

void Character::serialize(QIODevice *dev) const {
    IO::write_short(dev, m_id);

    IO::write_string(dev, m_name);

    IO::write_short(dev, m_color);

    IO::write_long(dev, m_activity.m_ms_left);
    IO::write_long(dev, m_activity.m_ms_total);
    IO::write_short(dev, m_activity.m_action);

    for (ItemDomain domain : { Material, Artifact }) {
        IO::write_short(dev, domain);
        const auto &ids = m_external_item_ids.at(domain);
        IO::write_short(dev, ids.size());
        for (size_t i = 0; i < ids.size(); i++) {
            IO::write_long(dev, ids[i]);
        }
    }

    IO::write_short(dev, m_effects.size());
    for (size_t i = 0; i < m_effects.size(); i++) {
        IO::write_item(dev, m_effects[i]);
    }

    for (ItemDomain domain : { SmithingTool, ForagingTool, MiningTool }) {
        IO::write_short(dev, domain);
        IO::write_long(dev, m_tool_ids.at(domain));
    }

    IO::write_short(dev, m_energy);

    IO::write_short(dev, m_morale);
}

// Transfers ownership
Character *Character::deserialize(QIODevice *dev) {
    Character *c = new Character;

    c->m_id = IO::read_short(dev);

    c->m_name = IO::read_string(dev);

    c->m_color = (Color) IO::read_short(dev);

    qint64 ms_left = IO::read_long(dev);
    qint64 ms_total = IO::read_long(dev);
    ItemDomain action = (ItemDomain) IO::read_short(dev);
    c->m_activity = CharacterActivity(c->m_id, action, ms_total, ms_left);

    for (int i = 0; i < 2; i++) {
        ItemDomain domain = (ItemDomain) IO::read_short(dev);
        quint16 size = IO::read_short(dev);
        for (size_t i = 0; i < size; i++) {
            c->m_external_item_ids[domain][i] = IO::read_long(dev);
        }
    }

    quint16 size = IO::read_short(dev);
    for (size_t i = 0; i < size; i++) {
        c->m_effects[i] = IO::read_item(dev);
    }

    for (int i = 0; i < 3; i++) {
        ItemDomain domain = (ItemDomain) IO::read_short(dev);
        c->m_tool_ids[domain] = IO::read_long(dev);
    }

    c->m_energy = IO::read_short(dev);

    c->m_morale = IO::read_short(dev);

    return c;
}
