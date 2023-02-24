#include "gamewindow.h"
#include "items.h"
#include "itemslot.h"
#include "externalslot.h"
#include "effectslot.h"
#include "queuedactivityslot.h"
#include "skillslot.h"
#include "encyclopedia.h"
#include "explorerbutton.h"
#include "die.h"
#include "main.h"
#include "about.h"
#include "menu.h"

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
      m_encyclopedia(new Encyclopedia),
      m_about_box(new AboutBox)
{
    m_window.setupUi(this);
    m_event_log.setupUi(new QDialog(this));

    setMenuBar(new MenuBar(this));

    Game *new_game = Game::new_game();
    m_game = *new_game;
    delete new_game;
    m_encyclopedia->refresh();

    const auto activity_buttons = get_activity_buttons();
    for (const auto &pair : activity_buttons) {
        connect(activity_buttons.at(pair.first), &QPushButton::clicked, [=]() {
            selected_char().queue_activity(pair.first, {});
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

    ItemSlot::insert_inventory_slots();
    ExternalSlot::insert_external_slots();
    ToolSlot::insert_tool_slots();
    EffectSlot::insert_effect_slots();
    PortraitSlot::insert_portrait_slot();
    ExplorerButton::insert_explorer_buttons();
    QueuedActivitySlot::insert_queued_activity_slots();
    SkillSlot::insert_skill_slots();

    QPalette activity_palette;
    activity_palette.setColor(QPalette::Highlight, Colors::qcolor(Lime));
    m_window.activity_time_bar->setPalette(activity_palette);

    QPalette spirit_palette;
    spirit_palette.setColor(QPalette::Highlight, Colors::qcolor(Blueberry));
    m_window.spirit_bar->setPalette(spirit_palette);

    QPalette energy_palette;
    energy_palette.setColor(QPalette::Highlight, Colors::qcolor(Cherry));
    m_window.energy_bar->setPalette(energy_palette);

    m_encyclopedia->refresh();

    m_backup_timer_id = startTimer(BACKUP_INTERVAL_MS);
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
    return m_game.character(selected_char_id());
}

CharacterId &LKGameWindow::selected_char_id() {
    if (m_selected_char_id == NOBODY) {
        m_selected_char_id = m_game.characters()[0].id();
    }

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

void LKGameWindow::notify(NotificationType type, const QString &msg) {
    m_event_log.events_list->addItem(new GameNotification(type, msg));
}

void LKGameWindow::refresh_ui() {
    m_window.player_name_label->setText(QString("Explorer <b>%1</b>").arg(selected_char().name()));
    m_window.tribe_name_label->setText(QString("Expedition <b>%1</b>").arg(m_game.tribe_name()));

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
    m_game.refresh_ui_bars(m_window.activity_time_bar, m_window.spirit_bar, m_window.energy_bar, m_selected_char_id);
}

void LKGameWindow::refresh_ui_buttons() {
    bool smithing_already_queued = std::any_of(
        begin(selected_char().activities()),
        end(selected_char().activities()),
        [](CharacterActivity &a) {
            return a.action() == Smithing;
    });

    for (ItemDomain domain : { Smithing, Foraging, Mining }) {
        if (selected_char().can_perform_action(domain) && domain == Smithing && !smithing_already_queued) {
            get_activity_buttons().at(domain)->setEnabled(true);
        } else if (selected_char().can_perform_action(domain)) {
            get_activity_buttons().at(domain)->setEnabled(true);
        } else {
            get_activity_buttons().at(domain)->setEnabled(false);
        }
    }

    m_window.trade_partner_combobox->setEnabled(m_game.trade_partner() == NOBODY);

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
            window().trade_notification_label->setText(QString("%1 is carrying out this trade...").arg(character.name()));
        }
    }
}

void LKGameWindow::tutorial(const QString &text) {
    QMessageBox tut;
    tut.setText(text);
    tut.setIcon(QMessageBox::Information);
    tut.exec();
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

ItemSlot *LKGameWindow::get_slot(const QString &name) {
    auto result = std::find_if(begin(m_slots), end(m_slots), [&name](ItemSlot *slot) {
        return slot->objectName() == name;
    });

    if (result == end(m_slots)) {
        bugcheck(ItemSlotByNameLookupMiss, name);
    }

    return *result;
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

    for (Character &character : m_game.characters()) {
        character.activities().front().start();
    }
}

bool LKGameWindow::save_file_exists() {
    return QFile::exists(SAVE_FILE_NAME);
}

void LKGameWindow::enter_multiwindow_mode() {
    std::vector<QWidget *> widgets_to_split = {
        findChild<QWidget *>("activities_widget"),
        findChild<QWidget *>("inventory_widget"),
        findChild<QWidget *>("explorer_widget")
    };

    for (QWidget *widget : widgets_to_split) {
        GameMultiWindow *window = new GameMultiWindow();
        window->setWindowTitle("Aegis of Rhodon");

        window->setCentralWidget(widget);
        window->setMenuBar(new MenuBar(this));
        window->show();
        window->setMinimumSize(QSize(window->size().width(), window->size().height())); // evil hack to prevent resizing
        window->setMaximumSize(QSize(window->size().width(), window->size().height()));

        m_multiwindows.push_back(window);
    }

    hide();
}

void LKGameWindow::exit_multiwindow_mode() {
    QGridLayout *central_layout = (QGridLayout *) m_window.super_widget->layout();
    central_layout->addWidget(m_multiwindows[0]->centralWidget(), 0, 0, 1, 2); // BRITTLE AS SHIT OH MAN
    central_layout->addWidget(m_multiwindows[1]->centralWidget(), 1, 0, 1, 1); // These are pulled directly from ui_main.h
    central_layout->addWidget(m_multiwindows[2]->centralWidget(), 1, 1, 1, 1); // WARNING: DANGER: TODO: YOU WILL GET FUCKED

    for (QWidget *window : m_multiwindows) {
        window->hide();
        window->deleteLater();
    }

    m_multiwindows.clear();

    show();
}

void LKGameWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_backup_timer_id) {
        if (m_save_file.exists()) {
            m_save_file.copy(SAVE_FILE_NAME + ".bak");
        }
    }

    for (Character &character : m_game.characters()) {
        if (character.activity().timer_id() == event->timerId() && character.activity().action() != None) {
            character.activity().progress(ACTIVITY_TICK_RATE_MS);
        }
    }

    refresh_ui_bars();
}

void LKGameWindow::closeEvent(QCloseEvent *event) {
    QMessageBox prompt;
    prompt.setText("Are you sure you want to quit?");
    prompt.setInformativeText("Your game will be saved.");
    prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    prompt.setIcon(QMessageBox::Question);
    if (prompt.exec() == QMessageBox::Yes) {
        save();
        event->accept();
    } else {
        event->ignore();
    }
}

LKGameWindow *gw() {
    return LKGameWindow::the_game_window;
}

void GameMultiWindow::closeEvent(QCloseEvent *event) {
    if (gw()->close()) {
        QCoreApplication::quit();
    } else {
        event->ignore();
    }
}
