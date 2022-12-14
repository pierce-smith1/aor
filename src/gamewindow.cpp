#include "gamewindow.h"
#include "items.h"
#include "itemslot.h"
#include "externalslot.h"
#include "effectslot.h"
#include "encyclopedia.h"
#include "explorerbutton.h"

LKGameWindow *LKGameWindow::the_game_window;

void LKGameWindow::instantiate_singleton() {
    std::allocator<LKGameWindow> alloc;
    the_game_window = alloc.allocate(1);
    alloc.construct(the_game_window);
}

LKGameWindow::LKGameWindow()
    : m_item_tooltip(new Tooltip()),
      m_connection(),
      m_save_file(SAVE_FILE_NAME),
      m_encyclopedia(new Encyclopedia())
{
    m_window.setupUi(this);

    const auto activity_buttons = get_activity_buttons();
    for (const auto &pair : activity_buttons) {
        connect(activity_buttons.at(pair.first), &QPushButton::clicked, [=]() {
            selected_char().start_activity(pair.first);
        });
    }

    connect(m_window.trade_accept_button, &QPushButton::clicked, [=]() {
        m_connection.agreement_changed(m_selected_tribe_id, true);
        m_game.accepting_trade() = true;
        refresh_ui_buttons();

        if (m_game.tribes().at(selected_tribe_id()).remote_accepted) {
            m_connection.execute_trade();
            m_connection.notify_trade(selected_tribe_id());
        }
    });

    connect(m_window.trade_unaccept_button, &QPushButton::clicked, [=]() {
        m_connection.agreement_changed(m_selected_tribe_id, false);
        m_game.accepting_trade() = false;
        m_game.trade_partner() = NOBODY;
        refresh_ui_buttons();
    });

    connect(m_window.trade_partner_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        m_selected_tribe_id = m_window.trade_partner_combobox->itemData(index).toLongLong(nullptr);
        refresh_ui();
    });

    connect(m_window.encyclopedia_action, &QAction::triggered, [=]() {
        m_encyclopedia->show();
    });

    ItemSlot::insert_inventory_slots();
    ExternalSlot::insert_external_slots();
    ToolSlot::insert_tool_slots();
    EffectSlot::insert_effect_slots();
    PortraitSlot::insert_portrait_slot();

    for (const auto &pair : m_game.characters()) {
        m_window.explorer_slots->layout()->addWidget(new ExplorerButton(this, pair.first));
    }

    notify(Discovery, "The Sun breaks on a new adventure.");

    QPalette activity_palette;
    activity_palette.setColor(QPalette::Highlight, Colors::qcolor(Lime));
    m_window.activity_time_bar->setPalette(activity_palette);

    QPalette morale_palette;
    morale_palette.setColor(QPalette::Highlight, Colors::qcolor(BlueRaspberry));
    m_window.morale_bar->setPalette(morale_palette);

    QPalette energy_palette;
    energy_palette.setColor(QPalette::Highlight, Colors::qcolor(Cherry));
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

Tooltip *&LKGameWindow::tooltip() {
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

void LKGameWindow::register_slot(ItemSlot *slot) {
    m_slots.push_back(slot);
}

void LKGameWindow::notify(NotificationType, const QString &message) {
    m_window.statusbar->showMessage(message);
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
    for (ItemSlot *slot : m_slots) {
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
        if (selected_char().can_perform_action(domain) && !selected_char().activity().ongoing()) {
            get_activity_buttons().at(domain)->setEnabled(true);
        } else {
            get_activity_buttons().at(domain)->setEnabled(false);
        }
    }

    if (!m_connection.is_connected()
        || selected_char().activity().ongoing()
        || trade_ongoing(m_selected_tribe_id)
        || m_window.trade_partner_combobox->count() == 0
    ) {
        m_window.trade_accept_button->setEnabled(false);
        m_window.trade_unaccept_button->setEnabled(false);
    } else if (m_game.accepting_trade()) {
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

    window().trade_arrow_label->setPixmap(QPixmap(":/assets/img/icons/arrows_disabled.png"));
    window().trade_notification_label->setText("");

    for (auto &pair : game().characters()) {
        if (pair.second.activity().action() == Trading) {
            window().trade_arrow_label->setPixmap(QPixmap(":/assets/img/icons/arrows.png"));
            window().trade_notification_label->setText(QString("%1 is carrying out this trade...").arg(selected_char().name()));
        }
    }
}

bool LKGameWindow::trade_ongoing(GameId tribe) {
    return m_game.trade_partner() == tribe;
}

const std::map<ItemDomain, QPushButton *> LKGameWindow::get_activity_buttons() {
    return {
        { Smithing, m_window.smith_button },
        { Foraging, m_window.forage_button },
        { Mining, m_window.mine_button },
    };
}

const std::vector<ItemSlot *> &LKGameWindow::item_slots() {
    return m_slots;
}

const std::vector<ItemSlot *> LKGameWindow::item_slots(ItemDomain domain) {
    std::vector<ItemSlot *> slots_of_type;

    for (ItemSlot *slot : item_slots()) {
        if (slot->type() == domain) {
            slots_of_type.push_back(slot);
        }
    }

    return slots_of_type;
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
    for (auto &pair : m_game.characters()) {
        if (pair.second.activity().timer_id() == event->timerId()) {
            pair.second.activity().progress(ACTIVITY_TICK_RATE_MS);
        }
    }

    refresh_ui_bars();
}

LKGameWindow *gw() {
    return LKGameWindow::the_game_window;
}

