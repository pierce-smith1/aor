#include "gamewindow.h"
#include "items.h"
#include "itemslot.h"
#include "externalslot.h"
#include "effectslot.h"
#include "state.h"

GameTimers::GameTimers(LKGameWindow *game)
    : game(game) { }

void GameTimers::timerEvent(QTimerEvent *event) {
    if (event->timerId() == activity_timer_id) {
        game->progress_activity(ACTIVITY_TICK_RATE_MS);
    }
}

LKGameWindow::LKGameWindow()
    : window(), timers(this)
{
    window.setupUi(this);

    const auto activity_buttons {get_activity_buttons()};
    for (const auto &pair : activity_buttons) {
        connect(activity_buttons.at(pair.first), &QPushButton::clicked, [=]() {
            start_activity(pair.first);
        });
    }

    ItemSlot::insert_inventory_slots(*this);
    ExternalSlot::insert_external_slots(*this);
    ToolSlot::insert_tool_slots(*this);
    EffectSlot::insert_effect_slots(*this);
    PortraitSlot::insert_portrait_slot(*this);

    activity_buttons.at(Smithing)->setEnabled(false);

    notify(Discovery, "The Sun breaks on a new adventure.");
}

void LKGameWindow::register_slot_name(const QString &slot_name) {
    slot_names.push_back(slot_name);
}

void LKGameWindow::mutate_state(std::function<void(State &)> action) {
    QMutexLocker lock(&mutex);

    action(character);
    refresh_ui();
}

void LKGameWindow::notify(NotificationType type, const QString &message) {
    window.event_list->addItem(new GameNotification(type, message));
}

void LKGameWindow::start_activity(ItemDomain type) {
    start_activity(CharacterActivity(type, 50000));
}

void LKGameWindow::start_activity(const CharacterActivity &activity) {
    QMutexLocker lock(&mutex);

    if (activity.action != None) {
        lock_ui();
        timers.activity_timer_id = timers.startTimer(ACTIVITY_TICK_RATE_MS);
    }

    character.activity = activity;
    refresh_ui();

    visual_energy = character.energy;
    visual_morale = character.morale;
}

void LKGameWindow::progress_activity(std::int64_t by_ms) {
    QMutexLocker lock(&mutex);

    character.activity.ms_left -= by_ms;

    if (character.activity.ms_left < 0) {
        complete_activity();
        return;
    }

    double percent_completed {((double) by_ms / character.activity.ms_total)};

    Item current_tool {character.get_item_instance(character.tool_ids[character.activity.action])};
    visual_energy -= current_tool.def()->properties[ToolEnergyCost]* percent_completed;

    if (character.activity.action == Eating) {
        std::vector foods {character.get_items_of_intent(Consumable)};
        int total_energy_gain {std::accumulate(
            begin(foods),
            end(foods),
            0,
            [](int a, const Item &b) {
                return b.def()->properties[ConsumableEnergyBoost] + a;
            }
        )};
        int total_morale_gain {std::accumulate(
            begin(foods),
            end(foods),
            0,
            [](int a, const Item &b) {
                return b.def()->properties[ConsumableMoraleBoost] + a;
            }
        )};
        visual_energy += total_energy_gain * percent_completed;
        visual_morale += total_morale_gain * percent_completed;
    }

    refresh_ui_bars();
}

void LKGameWindow::refresh_ui() {
    QMutexLocker lock(&mutex);

    for (const QString &slot_name : slot_names) {
        findChild<ItemSlot *>(slot_name)->refresh_pixmap();
    }

    // Disable other buttons if we don't have enough energy for their tool
    for (ItemDomain domain : { Smithing, Foraging, Mining, Praying }) {
        ItemDefinitionPtr tool_def = character.get_item_instance(character.tool_ids[domain]).def();
        if (tool_def->properties[ToolEnergyCost] <= character.energy && character.activity.action == None) {
            get_activity_buttons().at(domain)->setEnabled(true);
        } else {
            get_activity_buttons().at(domain)->setEnabled(false);
        }
    }

    // Disable smithing button if not enough materials
    const auto &materials = character.external_item_ids[Material];
    if (std::all_of(begin(materials), begin(materials) + SMITHING_SLOTS, [&](ItemId id) {
        return id != EMPTY_ID;
    }) && character.activity.action == None) {
        get_activity_buttons().at(Smithing)->setEnabled(true);
    } else {
        get_activity_buttons().at(Smithing)->setEnabled(false);
    }

    visual_energy = character.energy;
    visual_morale = character.morale;

    refresh_ui_bars();
}

void LKGameWindow::refresh_ui_bars() {
    window.energy_bar->setValue(visual_energy);
    window.energy_bar->setMaximum(BASE_MAX_ENERGY);

    window.morale_bar->setValue(visual_morale);
    window.morale_bar->setMaximum(BASE_MAX_MORALE);

    window.activity_time_bar->setValue(character.activity.ms_total - character.activity.ms_left);
    window.activity_time_bar->setMaximum(character.activity.ms_total);
}

bool LKGameWindow::activity_ongoing() {
    QMutexLocker lock(&mutex);

    return character.activity.action != None;
}

void LKGameWindow::complete_activity() {
    QMutexLocker lock(&mutex);

    auto drop_items_in_slots = [=](ItemDomain type) {
        for (const QString &slot_name : get_item_slot_names()) {
            ItemSlot *slot = findChild<ItemSlot *>(slot_name);
            if (slot->get_item_slot_type() & type) {
                slot->drop_external_item();
            }
        }
    };

    std::vector<Item> inputs;
    switch (character.activity.action) {
        case Smithing: {
            inputs = character.get_items_of_intent(Material);
            drop_items_in_slots(Material);
            notify(ActionComplete, "You finished smithing.");
            break;
        }
        case Foraging: {
            notify(ActionComplete, "You finished foraging.");
            break;
        }
        case Mining: {
            notify(ActionComplete, "You finished mining");
            break;
        }
        case Praying: {
            inputs = character.get_items_of_intent(Offering);
            drop_items_in_slots(Offering);
            notify(ActionComplete, "You finished your prayers.");
            break;
        }
        case Eating: {
            inputs = character.get_items_of_intent(Consumable);
            notify(ActionComplete, "You finished eating.");
            character.add_energy(std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return b.def()->properties[ConsumableEnergyBoost] + a;
            }));
            character.add_morale(std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return b.def()->properties[ConsumableMoraleBoost] + a;
            }));
            break;
        }
        default: break;
    }

    // Generate the items
    Item tool {character.get_item_instance(character.tool_ids[character.activity.action])};
    std::vector<Item> new_items {Actions::generate_items(inputs, tool, character.activity.action)};
    for (const Item &item : new_items) {
        bool add_successful {character.add_item(item)};
        if (!add_successful) {
            notify(Warning, "Your inventory was too full to recieve all of your new items!");
            break;
        }
        notify(Discovery, QString("You discovered a %1!").arg(item.def()->display_name));
    }

    // Dink all of the items used as inputs, unless we are praying, in which
    // case just eat them outright
    for (const Item &input : inputs) {
        Item &item {character.get_item_ref(input.id)};

        if (character.activity.action == Praying) {
            character.remove_item_with_id(input.id);
        } else {
            item.uses_left -= 1;
            if (item.uses_left == 0) {
                character.remove_item_with_id(input.id);
            }
        }
    }

    // Dink the tool and deduct its energy cost
    if (character.tool_ids[character.activity.action] != EMPTY_ID) {
        Item &tool {character.get_item_ref(character.tool_ids[character.activity.action])};
        tool.uses_left -= 1;
        if (tool.uses_left == 0) {
            notify(Warning, QString("Your %1 broke.").arg(tool.def()->display_name));
            drop_items_in_slots(Tool);
            character.remove_item_with_id(tool.id);
        }
        character.energy -= tool.def()->properties[ToolEnergyCost];
    }

    killTimer(timers.activity_timer_id);
    timers.activity_timer_id = 0;

    character.activity = CharacterActivity(None, 0);

    unlock_ui();
    refresh_ui();
}

const std::map<ItemDomain, QPushButton *> LKGameWindow::get_activity_buttons() {
    return {
        { Smithing, window.smith_button },
        { Foraging, window.forage_button },
        { Mining, window.mine_button },
        { Praying, window.pray_button },
    };
}

const std::vector<QString> &LKGameWindow::get_item_slot_names() {
    return slot_names;
}

void LKGameWindow::lock_ui() {
    for (const auto &pair : get_activity_buttons()) {
        pair.second->setEnabled(false);
    }
}

void LKGameWindow::unlock_ui() {
    for (const auto &pair : get_activity_buttons()) {
        pair.second->setEnabled(true);
    }
}
