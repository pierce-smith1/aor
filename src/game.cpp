#include "game.h"

Game::Game() {
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());

    add_item(Item("globfruit"));
    add_item(Item("globfruit"));
    add_item(Item("globfruit"));
}

Characters &Game::characters() {
    return m_explorers;
}

Inventory &Game::inventory() {
    return m_inventory;
}

void Game::add_character(const QString &name) {
    CharacterId max_id;
    auto max_id_search = std::max_element(begin(m_explorers), end(m_explorers), [&](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    if (max_id_search == end(m_explorers)) {
        m_explorers.emplace(0, Character(0, name, this));
    } else {
        max_id = max_id_search->first;
        m_explorers.emplace(max_id + 1, Character(max_id + 1, name, this));
    }

}

void Game::add_item(const Item &item) {
    m_inventory.add_item(item);
}

TooltipText Game::tooltip_text_for(const Item &item) {
    ItemDefinitionPtr this_def = item.def();
    QString character_name = item.intent_holder == NOBODY ? "" : m_explorers.at(item.intent_holder).name();

    TooltipText text;
    text.title = QString("<b>%1</b>").arg(this_def->display_name);
    text.description = this_def->description;

    switch (this_def->item_level) {
        case 1: { text.subtext = "Unremarkable "; break; }
        case 2: { text.subtext = "Common "; break; }
        case 3: { text.subtext = "Notable "; break; }
        case 4: { text.subtext = "Rare "; break; }
        case 5: { text.subtext = "Enchanted "; break; }
        case 6: { text.subtext = "Truly Extraordinary "; break; }
        case 7: { text.subtext = "Anomalous "; break; }
        case 8: { text.subtext = "Incomprehensible "; break; }
        default: { break; }
    }

    text.subtext += Item::type_to_string(this_def->type);

    switch (item.intent) {
        default:
        case None: {
            break;
        }
        case Consumable: {
            text.subtext += QString(" <b><font color=green>(Being eaten by %1)</font></b>").arg(character_name);
            break;
        }
        case Material: {
            text.subtext += QString(" <b><font color=green>(Queued for smithing by %1)</font></b>").arg(character_name);
            break;
        }
        case Offering: {
            text.subtext += QString(" <b><font color=green>(Queued for offering by %1)</font></b>").arg(character_name);
            break;
        }
        case KeyOffering: {
            text.subtext += QString(" <b><font color=#ff7933>(Queued as key offering by %1)</font></b>").arg(character_name);
            break;
        }
        case SmithingTool:
        case ForagingTool:
        case MiningTool:
        case PrayerTool:
        case Artifact: {
            text.subtext += QString(" <b><font color=green>(Equipped by %1)</font></b>").arg(character_name);
            break;
        }
    }

    text.description += "<br>" + Item::properties_to_string(this_def->properties);

    if (item.uses_left != 0) {
        text.subtext += QString(" <font color=gray>(%1 uses left)</font>").arg(item.uses_left);
    }

    return text;
}

void Game::refresh_ui_bars(QProgressBar *activity, QProgressBar *morale, QProgressBar *energy, CharacterId char_id) {
    Character &character = m_explorers.at(char_id);

    activity->setMaximum(100);
    activity->setValue(character.activity_percent_complete() * 100);

    double morale_gain = character.morale_to_gain() * character.activity_percent_complete();
    morale->setMaximum(character.max_morale());
    morale->setValue(character.morale() + morale_gain);

    double energy_gain = character.energy_to_gain() * character.activity_percent_complete();
    energy->setMaximum(character.max_energy());
    energy->setValue(character.energy() + energy_gain);
}
