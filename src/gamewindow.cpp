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
    window.player_name_label->setText(QString("Colonist <b>%1</b>").arg(character.name()));

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

    refresh_ui();
}

void LKGameWindow::register_slot_name(const QString &slot_name) {
    slot_names.push_back(slot_name);
}

void LKGameWindow::notify(NotificationType type, const QString &message) {
    window.event_list->addItem(new GameNotification(type, message));
}

void LKGameWindow::start_activity(ItemDomain type) {
    start_activity(CharacterActivity(type, 50000));
}

void LKGameWindow::start_activity(const CharacterActivity &activity) {
    if (activity.action != None) {
        lock_ui();
        timers.activity_timer_id = timers.startTimer(ACTIVITY_TICK_RATE_MS);
    }

    character.activity() = activity;
    refresh_ui();

    visual_energy = character.energy();
    visual_morale = character.morale();
}

void LKGameWindow::progress_activity(std::int64_t by_ms) {
    character.activity().ms_left -= by_ms;

    if (character.activity().ms_left < 0) {
        complete_activity();
        return;
    }

    double percent_completed {((double) by_ms / character.activity().ms_total)};

    visual_energy += character.energy_gain() * percent_completed;
    visual_morale += character.morale_gain() * percent_completed;

    refresh_ui_bars();
}

void LKGameWindow::refresh_ui() {
    for (const QString &slot_name : slot_names) {
        findChild<ItemSlot *>(slot_name)->refresh_pixmap();
    }

    visual_energy = character.energy();
    visual_morale = character.morale();

    refresh_ui_buttons();
    refresh_ui_bars();
}

void LKGameWindow::refresh_ui_bars() {
    window.energy_bar->setValue(visual_energy);
    window.energy_bar->setMaximum(character.max_energy());

    window.morale_bar->setValue(visual_morale);
    window.morale_bar->setMaximum(character.max_morale());

    window.activity_time_bar->setValue(character.activity().ms_total - character.activity().ms_left);
    window.activity_time_bar->setMaximum(character.activity().ms_total);
}

void LKGameWindow::refresh_ui_buttons() {
    for (ItemDomain domain : { Smithing, Foraging, Mining, Praying }) {
        if (character.can_do(domain)) {
            get_activity_buttons().at(domain)->setEnabled(true);
        } else {
            get_activity_buttons().at(domain)->setEnabled(false);
        }
    }
}

void LKGameWindow::complete_activity() {
    auto drop_items_in_slots = [=](ItemDomain type) {
        for (const QString &slot_name : get_item_slot_names()) {
            ItemSlot *slot = findChild<ItemSlot *>(slot_name);
            if (slot->get_item_slot_type() & type) {
                slot->drop_external_item();
            }
        }
    };

    std::vector<Item> inputs = character.inputs();
    switch (character.activity().action) {
        case Smithing: {
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
            drop_items_in_slots(Offering);
            drop_items_in_slots(KeyOffering);
            notify(ActionComplete, "You finished your prayers.");
            break;
        }
        case Eating: {
            notify(ActionComplete, "You finished eating.");
            break;
        }
        default: {
            break;
        }
    }

    // Generate the items
    Item tool {character.tool(character.activity().action)};
    std::vector<Item> new_items {Actions::generate_items(inputs, tool, character.activity().action)};
    for (const Item &item : new_items) {
        bool add_successful {character.give_item(item)};
        if (!add_successful) {
            notify(Warning, "Your inventory was too full to recieve all of your new items!");
            break;
        }
        notify(Discovery, QString("You discovered a %1!").arg(item.def()->display_name));
    }

    // Dink all of the items used as inputs, unless we are praying, in which
    // case just eat them outright
    for (const Item &input : inputs) {
        Item &item {character.item_ref(input.id)};

        if (character.activity().action == Praying) {
            character.remove_item(input.id);
        } else {
            item.uses_left -= 1;
            if (item.uses_left == 0) {
                character.remove_item(input.id);
            }
        }
    }

    // Dink the tool
    if (character.tool(character.activity().action).id != EMPTY_ID) {
        Item &tool {character.item_ref(character.tool(character.activity().action).id)};
        tool.uses_left -= 1;
        if (tool.uses_left == 0) {
            notify(Warning, QString("Your %1 broke.").arg(tool.def()->display_name));
            drop_items_in_slots(Tool);
            character.remove_item(tool.id);
        }
    }

    character.add_energy(character.energy_gain());
    character.add_morale(character.morale_gain());

    killTimer(timers.activity_timer_id);
    timers.activity_timer_id = 0;

    character.activity() = CharacterActivity(None, 0);

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
