#include "gamewindow.h"
#include "items.h"
#include "itemslot.h"
#include "externalslot.h"
#include "effectslot.h"
#include "encyclopedia.h"
#include "explorerbutton.h"
#include "die.h"
#include "main.h"

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
    m_initialized = true;

    Game *new_game = Game::new_game();
    m_game = *new_game;
    delete new_game;

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
    ExplorerButton::insert_explorer_buttons();

    notify(Discovery, "The Sun breaks on a new adventure.");

    QPalette activity_palette;
    activity_palette.setColor(QPalette::Highlight, Colors::qcolor(Lime));
    m_window.activity_time_bar->setPalette(activity_palette);

    QPalette morale_palette;
    morale_palette.setColor(QPalette::Highlight, Colors::qcolor(Blueberry));
    m_window.morale_bar->setPalette(morale_palette);

    QPalette energy_palette;
    energy_palette.setColor(QPalette::Highlight, Colors::qcolor(Cherry));
    m_window.energy_bar->setPalette(energy_palette);

    m_encyclopedia->refresh();

    m_window.statusbar->showMessage("aor " + GAME_VERSION);
}

bool LKGameWindow::initialized() {
    return m_initialized;
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

Encyclopedia *&LKGameWindow::encyclopedia() {
    return m_encyclopedia;
}

void LKGameWindow::register_slot(ItemSlot *slot) {
    m_slots.push_back(slot);
}

void LKGameWindow::notify(NotificationType, const QString &message) { }

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
        || m_game.foreign_trade_level(m_selected_tribe_id) != m_game.trade_level()
        || m_game.trade_level() == 0
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

    if (m_selected_tribe_id != NOBODY) {
        m_window.foreign_trade_level_label->setText(QString("<b>%1</b>")
            .arg(m_game.foreign_trade_level(m_selected_tribe_id))
        );
    } else {
        m_window.foreign_trade_level_label->setText("");
    }

    m_window.trade_level_label->setText(QString("<b>%1</b>").arg(m_game.trade_level()));

    if (m_game.foreign_trade_level(m_selected_tribe_id) == m_game.trade_level()) {
        m_window.foreign_trade_level_label->setText(
            "<font color=green>" + m_window.foreign_trade_level_label->text() + "</font>"
        );

        m_window.trade_level_label->setText(
            "<font color=green>" + m_window.trade_level_label->text() + "</font>"
        );
    }

    window().trade_arrow_label->setPixmap(QPixmap(":/assets/img/icons/arrows_disabled.png"));
    window().trade_notification_label->setText("");

    for (Character &character : game().characters()) {
        if (character.activity().action() == Trading) {
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
    if (m_save_file.openMode() == QIODevice::NotOpen) {
        m_save_file.open(QIODevice::ReadWrite);
    }

    m_save_file.reset();

    IO::write_byte(&m_save_file, 'r');
    IO::write_byte(&m_save_file, 'h');
    IO::write_byte(&m_save_file, 'o');

    m_game.serialize(&m_save_file);

    m_save_file.flush();
}

void LKGameWindow::load() {
    if (m_save_file.openMode() == QIODevice::NotOpen) {
        m_save_file.open(QIODevice::ReadWrite);
    }

    m_save_file.reset();

    char r = IO::read_byte(&m_save_file);
    char h = IO::read_byte(&m_save_file);
    char o = IO::read_byte(&m_save_file);

    if (r != 'r' || h != 'h' || o != 'o') {
        bugcheck(SaveInvalidHeader, r, h, o);
    }

    Game *game = Game::deserialize(&m_save_file);
    m_game = *game;
    delete game;
}

bool LKGameWindow::save_file_exists() {
    return QFile::exists(SAVE_FILE_NAME);
}

void LKGameWindow::timerEvent(QTimerEvent *event) {
    for (Character &character : m_game.characters()) {
        if (character.activity().timer_id() == event->timerId() && character.activity().action() != None) {
            character.activity().progress(ACTIVITY_TICK_RATE_MS);
        }
    }

    refresh_ui_bars();
}

void LKGameWindow::closeEvent(QCloseEvent *event) {
    save();
    event->accept();
}

LKGameWindow *gw() {
    return LKGameWindow::the_game_window;
}
