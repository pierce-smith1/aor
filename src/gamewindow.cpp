#include "gamewindow.h"
#include "items.h"
#include "itemslot.h"
#include "externalslot.h"
#include "effectslot.h"

LKGameWindow::LKGameWindow()
    : m_connection(this), m_save_file(SAVE_FILE_NAME)
{
    m_window.setupUi(this);

    const auto activity_buttons {get_activity_buttons()};
    for (const auto &pair : activity_buttons) {
        connect(activity_buttons.at(pair.first), &QPushButton::clicked, [=]() {
            start_activity(m_selected_char_id, pair.first);
        });
    }

    connect(m_window.trade_accept_button, &QPushButton::clicked, [=]() {
        m_connection.agreement_changed(m_selected_tribe_id, true);
        selected_char().accepting_trade() = true;
        refresh_ui_buttons();

        if (m_game.tribes().at(selected_tribe_id()).remote_accepted) {
            m_connection.execute_trade();
            m_connection.notify_trade(selected_tribe_id());
        }
    });

    connect(m_window.trade_unaccept_button, &QPushButton::clicked, [=]() {
        m_connection.agreement_changed(m_selected_tribe_id, false);
        selected_char().accepting_trade() = false;
        refresh_ui_buttons();
    });

    connect(m_window.trade_partner_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        m_selected_tribe_id = m_window.trade_partner_combobox->itemData(index).toLongLong(nullptr);
        refresh_ui();
    });

    ItemSlot::insert_inventory_slots(*this);
    ExternalSlot::insert_external_slots(*this);
    ToolSlot::insert_tool_slots(*this);
    EffectSlot::insert_effect_slots(*this);
    PortraitSlot::insert_portrait_slot(*this);

    for (const auto &pair : m_game.characters()) {
        m_window.explorer_slots->layout()->addWidget(new ExplorerButton(this, this, pair.first));
    }

    notify(Discovery, "The Sun breaks on a new adventure.");

    QPalette activity_palette;
    activity_palette.setColor(QPalette::Highlight, QColor(102, 204, 51));
    m_window.activity_time_bar->setPalette(activity_palette);

    QPalette morale_palette;
    morale_palette.setColor(QPalette::Highlight, QColor(0, 153, 255));
    m_window.morale_bar->setPalette(morale_palette);

    QPalette energy_palette;
    energy_palette.setColor(QPalette::Highlight, QColor(255, 51, 0));
    m_window.energy_bar->setPalette(energy_palette);

    m_save_file.open(QIODevice::ReadWrite);
}

Game &LKGameWindow::game() {
    return m_game;
}

DoughbyteConnection &LKGameWindow::connection() {
    return m_connection;
}

Ui::LKMainWindow &LKGameWindow::window() {
    return m_window;
}

Tooltip &LKGameWindow::tooltip() {
    return m_item_tooltip;
}

Character &LKGameWindow::selected_char() {
    return m_game.characters().at(m_selected_char_id);
}

CharacterId &LKGameWindow::selected_char_id() {
    return m_selected_char_id;
}

GameId &LKGameWindow::selected_tribe_id() {
    return m_selected_tribe_id;
}

void LKGameWindow::register_slot_name(const QString &slot_name) {
    m_slot_names.push_back(slot_name);
}

void LKGameWindow::notify(NotificationType, const QString &message) {
    m_window.statusbar->showMessage(message);
}

void LKGameWindow::start_activity(CharacterId char_id, ItemDomain type) {
    start_activity(char_id, CharacterActivity(type, 50000));
}

void LKGameWindow::start_activity(const CharacterActivity &activity) {
    start_activity(m_selected_char_id, activity);
}

void LKGameWindow::start_activity(CharacterId char_id, const CharacterActivity &activity) {
    Character &character = m_game.characters().at(char_id);

    if (activity.action != None) {
        m_timers[char_id] = startTimer(ACTIVITY_TICK_RATE_MS);
    }

    character.activity() = activity;
    refresh_ui();
}

void LKGameWindow::progress_activity(CharacterId char_id, std::int64_t by_ms) {
    Character &character = m_game.characters().at(char_id);

    character.activity().ms_left -= by_ms;
    if (character.activity().ms_left < 0) {
        complete_activity(char_id);
        return;
    }

    refresh_ui_bars();
}

void LKGameWindow::refresh_ui() {
    m_window.player_name_label->setText(QString("Explorer <b>%1</b>").arg(selected_char().name()));
    m_window.tribe_name_label->setText(QString("of <b>%1</b>").arg(m_game.tribe_name()));

    refresh_slots();
    refresh_ui_buttons();
    refresh_ui_bars();
    refresh_trade_ui();
}

void LKGameWindow::refresh_slots() {
    for (const QString &slot_name : m_slot_names) {
        ItemSlot *slot = findChild<ItemSlot *>(slot_name);
        slot->refresh_pixmap();
    }
}

void LKGameWindow::refresh_ui_bars() {
    m_game.refresh_ui_bars(m_window.activity_time_bar, m_window.morale_bar, m_window.energy_bar, m_selected_char_id);

    for (const auto &pair : m_game.characters()) {
        findChild<ExplorerButton *>(QString("explorer_button;%1").arg(pair.first))->refresh();
    }
}

void LKGameWindow::refresh_ui_buttons() {
    for (ItemDomain domain : { Smithing, Foraging, Mining }) {
        if (selected_char().can_perform_action(domain)) {
            get_activity_buttons().at(domain)->setEnabled(true);
        } else {
            get_activity_buttons().at(domain)->setEnabled(false);
        }
    }

    if (!m_connection.is_connected()
        || selected_char().activity_ongoing()
        || m_window.trade_partner_combobox->count() == 0
    ) {
        m_window.trade_accept_button->setEnabled(false);
        m_window.trade_unaccept_button->setEnabled(false);
    } else if (selected_char().accepting_trade()) {
        m_window.trade_accept_button->setEnabled(false);
        m_window.trade_unaccept_button->setEnabled(true);
    } else {
        m_window.trade_accept_button->setEnabled(true);
        m_window.trade_unaccept_button->setEnabled(false);
    }
}

void LKGameWindow::refresh_trade_ui() {
    if (m_selected_tribe_id != NOBODY && m_game.tribes().at(m_selected_tribe_id).remote_accepted) {
        m_window.trade_remote_accept_icon->setPixmap(QPixmap(":/assets/img/icons/check.png"));
    } else {
        m_window.trade_remote_accept_icon->setPixmap(QPixmap(":/assets/img/icons/warning.png"));
    }
}

void LKGameWindow::complete_activity(CharacterId char_id) {
    Character &character = m_game.characters().at(char_id);
    ItemDomain domain = character.activity().action;

    auto destroy_items_in_slots = [=](ItemDomain type) {
        for (ItemId &item_id : m_game.characters().at(char_id).external_items().at(type)) {
            item_id = EMPTY_ID;
        }
    };

    character.add_energy(character.energy_to_gain());
    character.add_morale(character.morale_to_gain());

    if (domain == Eating) {
        for (const Item &consumable : character.input_items()) {
            character.push_effect(Item(consumable.def()->properties[ConsumableGivesEffect]));
            for (int i = 0; i < consumable.def()->properties[ConsumableClearsNumEffects]; i++) {
                character.clear_last_effect();
            }
        }
    }

    // Generate the items
    Item tool = m_game.inventory().get_item(character.tools()[domain]);
    std::vector<Item> new_items = Generators::base_items(character.input_items(), tool, domain);
    for (const Item &item : new_items) {
        bool add_successful = m_game.inventory().add_item(item);
        if (!add_successful) {
            notify(Warning, QString("The inventory was too full to recieve all of %1's new items!").arg(character.name()));
            break;
        }
        notify(Discovery, QString("%2 discovered a %1!").arg(item.def()->display_name).arg(character.name()));
    }

    // Dink all of the items used as inputs
    for (const Item &input : character.input_items()) {
        Item &item = m_game.inventory().get_item_ref(input.id);

        if (item.uses_left != 0) {
            item.uses_left -= 1;
            if (item.uses_left == 0) {
                m_game.inventory().remove_item(input.id);
            }
        }
    }

    // Dink the tool
    if (character.tool_id() != EMPTY_ID) {
        Item &tool = m_game.inventory().get_item_ref(selected_char().tool_id());
        if (tool.uses_left != 0) {
            tool.uses_left -= 1;
            if (tool.uses_left == 0) {
                notify(Warning, QString("%2's %1 broke.").arg(tool.def()->display_name).arg(character.name()));
                destroy_items_in_slots(character.activity().action);
                m_game.inventory().remove_item(tool.id);
            }
        }
    }

    switch (character.activity().action) {
        case Smithing: {
            destroy_items_in_slots(Material);
            notify(ActionComplete, QString("%1 finished smithing.").arg(character.name()));
            break;
        }
        case Foraging: {
            notify(ActionComplete, QString("%1 finished foraging.").arg(character.name()));
            break;
        }
        case Mining: {
            notify(ActionComplete, QString("%1 finished mining.").arg(character.name()));
            break;
        }
        case Trading: {
            destroy_items_in_slots(Offering);
            notify(ActionComplete, QString("%1 finished trading.").arg(character.name()));
            break;
        }
        case Eating: {
            notify(ActionComplete, QString("%1 finished eating.").arg(character.name()));
            break;
        }
        default: {
            break;
        }
    }

    killTimer(m_timers[char_id]);
    m_timers[char_id] = 0;

    character.activity() = CharacterActivity(None, 0);

    refresh_ui();
}

const std::map<ItemDomain, QPushButton *> LKGameWindow::get_activity_buttons() {
    return {
        { Smithing, m_window.smith_button },
        { Foraging, m_window.forage_button },
        { Mining, m_window.mine_button },
    };
}

const std::vector<QString> &LKGameWindow::item_slot_names() {
    return m_slot_names;
}

void LKGameWindow::save() {
    m_save_file.reset();

    IO::write_byte(&m_save_file, 'l');
    IO::write_byte(&m_save_file, 'k');
    IO::write_byte(&m_save_file, 'i');

    m_game.serialize(&m_save_file);

    m_save_file.flush();
}

void LKGameWindow::load() {
    m_save_file.reset();

    char l = IO::read_byte(&m_save_file);
    char k = IO::read_byte(&m_save_file);
    char i = IO::read_byte(&m_save_file);

    if (l != 'l' || k != 'k' || i != 'i') {
        qFatal("save file is corrupt");
    }

    Game *game = Game::deserialize(&m_save_file);
    m_game = *game;
    delete game;
}

void LKGameWindow::timerEvent(QTimerEvent *event) {
    for (const auto &pair : m_timers) {
        if (event->timerId() == pair.second) {
            progress_activity(pair.first, ACTIVITY_TICK_RATE_MS);
        }
    }
}
