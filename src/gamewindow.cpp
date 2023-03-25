#include <QScrollBar>

#include "gamewindow.h"
#include "items.h"
#include "encyclopedia.h"
#include "die.h"
#include "main.h"
#include "about.h"
#include "menu.h"

#include "slot/inventoryslot.h"
#include "slot/materialslot.h"
#include "slot/toolslot.h"
#include "slot/artifactslot.h"
#include "slot/effectslot.h"
#include "slot/portraitslot.h"
#include "slot/queuedactivityslot.h"
#include "slot/smithingresultslot.h"
#include "slot/foreigntradeslot.h"
#include "slot/tradeslot.h"
#include "slot/skillslot.h"
#include "slot/explorerbutton.h"
#include "slot/weathereffectslot.h"
#include "slot/studyslot.h"

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
      m_about_box(new AboutBox),
      m_map_view(new MapView)
{
    m_window.setupUi(this);
    m_event_log.setupUi(new QDialog(this));

    window().crawl_contents->layout()->addWidget(m_map_view);
    window().crawl_area->verticalScrollBar()->setEnabled(false);

    Game *new_game = Game::new_game();
    m_game = new_game;

    setMenuBar(new MenuBar(this));

    const auto activity_buttons = get_activity_buttons();
    for (const auto &pair : activity_buttons) {
        connect(activity_buttons.at(pair.first), &QPushButton::clicked, [=]() {
            selected_char().queue_activity(pair.first, {});
        });
    }

    connect(m_window.trade_accept_button, &QPushButton::clicked, [=]() {
        m_connection.agreement_changed(m_selected_tribe_id, true);
        m_game->accepting_trade() = true;
        refresh_ui_buttons();

        if (m_game->tribes().at(selected_tribe_id()).remote_accepted) {
            m_connection.execute_trade();
            m_connection.notify_trade(selected_tribe_id());
        }
    });

    connect(m_window.trade_unaccept_button, &QPushButton::clicked, [=]() {
        m_connection.agreement_changed(m_selected_tribe_id, false);
        m_game->accepting_trade() = false;
        m_game->trade_partner() = NO_TRIBE;
        refresh_ui_buttons();
    });

    connect(m_window.trade_partner_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        m_selected_tribe_id = m_window.trade_partner_combobox->itemData(index).toLongLong();
        refresh_ui();
    });

    connect(m_window.study_chart_button, &QPushButton::clicked, [=]() {
        m_game->start_scan();
    });

    install_slots();

    QPalette activity_palette;
    activity_palette.setColor(QPalette::Highlight, Colors::qcolor(Lime));
    m_window.activity_time_bar->setPalette(activity_palette);

    QPalette spirit_palette;
    spirit_palette.setColor(QPalette::Highlight, Colors::qcolor(Blueberry));
    m_window.spirit_bar->setPalette(spirit_palette);

    QPalette energy_palette;
    energy_palette.setColor(QPalette::Highlight, Colors::qcolor(Cherry));
    m_window.energy_bar->setPalette(energy_palette);
    m_window.global_action_bar->setPalette(energy_palette);

    m_encyclopedia->refresh();

    m_backup_timer_id = startTimer(BACKUP_INTERVAL_MS);
    m_refresh_timer_id = startTimer(ACTIVITY_TICK_RATE_MS);

    CharacterActivity::empty_activity = new CharacterActivity();

    CharacterActivity::refresh_ui_bars(selected_char());
    m_window.map_progress_bar->setValue(0);
    m_window.lore_bar->setValue(m_game->lore().amount());
}

bool LKGameWindow::initialized() {
    return m_initialized;
}

Game *LKGameWindow::game() {
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
    return m_game->character(selected_char_id());
}

CharacterId &LKGameWindow::selected_char_id() {
    if (m_selected_char_id == NOBODY) {
        m_selected_char_id = m_game->characters()[0].id();
    }

    return m_selected_char_id;
}

GameId &LKGameWindow::selected_tribe_id() {
    return m_selected_tribe_id;
}

Encyclopedia *&LKGameWindow::encyclopedia() {
    return m_encyclopedia;
}

void LKGameWindow::register_slot(Slot *slot) {
    m_slots.push_back(slot);
}

void LKGameWindow::unregister_slot(Slot *slot) {
    m_slots.erase(std::find_if(m_slots.begin(), m_slots.end(), [=](Slot *s) {
        return s == slot;
    }));
}

void LKGameWindow::install_slots() {
    for (AorUInt x = 0; x < INVENTORY_COLS; x++) {
        for (AorUInt y = 0; y < INVENTORY_ROWS; y++) {
            (new InventorySlot(y, x))->install();
        }
    }

    for (AorUInt i = 0; i < SMITHING_SLOTS; i++) {
        (new MaterialSlot(i))->install();
    }

    (new ToolSlot(SmithingTool))->install();
    (new ToolSlot(ForagingTool))->install();
    (new ToolSlot(MiningTool))->install();

    for (AorUInt i = 0; i < ARTIFACT_SLOTS; i++) {
        (new ArtifactSlot(i))->install();
    }

    for (AorUInt i = 0; i < EFFECT_SLOTS; i++) {
        (new EffectSlot(i))->install();
    }

    for (AorUInt i = 0; i < MAX_QUEUED_ACTIVITIES; i++) {
        (new QueuedActivitySlot(i))->install();
    }

    for (AorUInt i = 0; i < MAX_EXPLORERS; i++) {
        (new ExplorerButton(i))->install();
    }

    (new PortraitSlot())->install();

    for (AorUInt i = 0; i < MAX_SKILLS; i++) {
        (new SkillSlot(i))->install();
    }

    (new SmithingResultSlot())->install();

    for (AorUInt i = 0; i < TRADE_SLOTS; i++) {
        (new ForeignTradeSlot(i))->install();
        (new TradeSlot(i))->install();
    }

    for (AorUInt i = 0; i < WEATHER_EFFECTS; i++) {
        (new WeatherEffectSlot(i))->install();
    }

    for (AorUInt i = 0; i < STUDY_SLOTS_PER_DOMAIN; i++) {
        for (ItemDomain d : { Consumable, Tool, Artifact }) {
            (new StudySlot(d, i))->install();
        }
    }
}

void LKGameWindow::notify(NotificationType type, const QString &msg) {
    m_event_log.events_list->addItem(new GameNotification(type, msg));
}

void LKGameWindow::refresh_ui() {
    m_window.player_name_label->setText(QString("Explorer <b>%1</b>").arg(selected_char().name()));
    m_window.tribe_name_label->setText(QString("Expedition <b>%1</b>").arg(m_game->tribe_name()));

    refresh_slots();
    refresh_ui_buttons();
    refresh_trade_ui();
    refresh_material_infostrips();
    refresh_global_action_bar();
    m_map_view->refresh();
}

void LKGameWindow::refresh_slots() {
    for (Slot *slot : m_slots) {
        slot->refresh();
    }
}

void LKGameWindow::refresh_ui_buttons() {
    auto activities = selected_char().activities();
    bool smithing_already_queued = std::any_of(activities.begin(), activities.end(), [](CharacterActivity *a) {
        return a->action() == Smithing;
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

    get_activity_buttons().at(Foraging)->setText(QString("Forage (%1)")
        .arg(m_game->forageables_left())
    );

    get_activity_buttons().at(Mining)->setText(QString("Mine (%1)")
        .arg(m_game->mineables_left())
    );

    m_window.trade_partner_combobox->setEnabled(m_game->trade_partner() == NO_TRIBE);

    if (!m_connection.is_connected()
        || selected_char().activity()->isActive()
        || trade_ongoing(m_selected_tribe_id)
        || m_window.trade_partner_combobox->count() == 0
        || m_game->foreign_trade_level(m_selected_tribe_id) != m_game->trade_level()
        || m_game->trade_level() == 0
    ) {
        m_window.trade_accept_button->setEnabled(false);
        m_window.trade_unaccept_button->setEnabled(false);
    } else if (m_game->accepting_trade()) {
        m_window.trade_accept_button->setEnabled(false);
        m_window.trade_unaccept_button->setEnabled(true);
    } else {
        m_window.trade_accept_button->setEnabled(true);
        m_window.trade_unaccept_button->setEnabled(false);
    }

    m_window.study_chart_button->setEnabled(m_game->can_scan());
}

void LKGameWindow::refresh_trade_ui() {
    if (m_selected_tribe_id != NO_TRIBE && m_game->tribes().at(m_selected_tribe_id).remote_accepted) {
        m_window.trade_remote_accept_icon->setPixmap(QPixmap(":/assets/img/icons/check.png"));
    } else {
        m_window.trade_remote_accept_icon->setPixmap(QPixmap(":/assets/img/icons/warning.png"));
    }

    if (m_selected_tribe_id != NO_TRIBE) {
        m_window.foreign_trade_level_label->setText(QString("<b>%1</b>")
            .arg(m_game->foreign_trade_level(m_selected_tribe_id))
        );
    } else {
        m_window.foreign_trade_level_label->setText("");
    }

    m_window.trade_level_label->setText(QString("<b>%1</b>").arg(m_game->trade_level()));

    if (m_game->foreign_trade_level(m_selected_tribe_id) == m_game->trade_level()) {
        m_window.foreign_trade_level_label->setText(
            "<font color=green>" + m_window.foreign_trade_level_label->text() + "</font>"
        );

        m_window.trade_level_label->setText(
            "<font color=green>" + m_window.trade_level_label->text() + "</font>"
        );
    }

    window().trade_arrow_label->setPixmap(QPixmap(":/assets/img/icons/arrows_disabled.png"));
    window().trade_notification_label->setText("");

    if (!m_connection.is_connected()) {
        window().trade_notification_label->setText("No connection to trade server - try restarting the game");
    } else if (m_game->tribes().size() == 1) { // Not == 0, since there's always a Nobody tribe
        window().trade_notification_label->setText("There's no one else to trade with.");
    }

    for (Character &character : m_game->characters()) {
        if (character.activity()->action() == Trading) {
            window().trade_arrow_label->setPixmap(QPixmap(":/assets/img/icons/arrows.png"));
            window().trade_notification_label->setText(QString("%1 is carrying out this trade...").arg(character.name()));
        }
    }
}

void LKGameWindow::refresh_material_infostrips() {
    ItemProperties total = m_game->total_resources();

    m_window.total_stone_label->setText(QString("<b>%1</b>").arg(total[StoneResource]));
    m_window.total_metallic_label->setText(QString("<b>%1</b>").arg(total[MetallicResource]));
    m_window.total_crystalline_label->setText(QString("<b>%1</b>").arg(total[CrystallineResource]));
    m_window.total_runic_label->setText(QString("<b>%1</b>").arg(total[RunicResource]));
    m_window.total_leafy_label->setText(QString("<b>%1</b>").arg(total[LeafyResource]));

    ItemProperties smith = m_game->total_smithing_resources(selected_char_id());

    m_window.total_smith_stone_label->setText(QString("<b>%1</b>").arg(smith[StoneResource]));
    m_window.total_smith_metallic_label->setText(QString("<b>%1</b>").arg(smith[MetallicResource]));
    m_window.total_smith_crystalline_label->setText(QString("<b>%1</b>").arg(smith[CrystallineResource]));
    m_window.total_smith_runic_label->setText(QString("<b>%1</b>").arg(smith[RunicResource]));
    m_window.total_smith_leafy_label->setText(QString("<b>%1</b>").arg(smith[LeafyResource]));
}

void LKGameWindow::refresh_global_action_bar() {
    m_window.global_action_bar->setMaximum(ACTIONS_UNTIL_WELCHIAN);
    m_window.global_action_bar->setValue(m_game->actions_done() > ACTIONS_UNTIL_WELCHIAN ? ACTIONS_UNTIL_WELCHIAN : m_game->actions_done());
    m_window.global_action_count->setText(QString("<b>%1</b>").arg(m_game->actions_done()));
    m_window.global_action_max->setText(QString("<b>%1</b>").arg(ACTIONS_UNTIL_WELCHIAN));
}

void LKGameWindow::tutorial(const QString &text) {
    QMessageBox tut;
    tut.setText(text);
    tut.setIcon(QMessageBox::Information);
    tut.exec();
}

bool LKGameWindow::trade_ongoing(GameId tribe) {
    return m_game->trade_partner() == tribe;
}

const std::map<ItemDomain, QPushButton *> LKGameWindow::get_activity_buttons() {
    return {
        { Smithing, m_window.smith_button },
        { Foraging, m_window.forage_button },
        { Mining, m_window.mine_button },
    };
}

void LKGameWindow::save() {
    if (m_save_file.openMode() == QIODevice::NotOpen) {
        m_save_file.open(QIODevice::ReadWrite);
    }

    m_save_file.reset();

    IO::write_uint(&m_save_file, SAVE_MAGIC_NUMBER);
    IO::write_uint(&m_save_file, MAJOR_VERSION);
    IO::write_uint(&m_save_file, MINOR_VERSION);
    IO::write_uint(&m_save_file, PATCH_VERSION);

    m_game->serialize(&m_save_file);

    m_save_file.flush();
}

void LKGameWindow::load() {
    if (m_save_file.openMode() == QIODevice::NotOpen) {
        m_save_file.open(QIODevice::ReadWrite);
    }

    m_save_file.reset();

    AorUInt magic = IO::read_uint(&m_save_file);

    if (magic != SAVE_MAGIC_NUMBER) {
        QMessageBox::critical(this, "Aegis of Rhodon", QString("The save file is damaged or of an incompatible version (1.x.x)."));
        exit(0);
    }

    AorUInt mv = IO::read_uint(&m_save_file);
    if (mv != MAJOR_VERSION) {
        QMessageBox::critical(this, "Aegis of Rhodon", QString("The save file is of an incompatible version."));
        exit(0);
    }

    IO::read_uint(&m_save_file);
    IO::read_uint(&m_save_file);

    m_game = new Game();
    m_game->deserialize(&m_save_file);

    for (Character &character : m_game->characters()) {
        character.activities().front()->start();
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
        //window->setMinimumSize(QSize(window->size().width(), window->size().height())); // evil hack to prevent resizing
        //window->setMaximumSize(QSize(window->size().width(), window->size().height()));

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

    setMenuBar(new MenuBar(this)); // LOL

    show();
}

void LKGameWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_backup_timer_id) {
        if (m_save_file.exists()) {
            m_save_file.copy(SAVE_FILE_NAME + ".bak");
        }
    }

    if (event->timerId() == m_refresh_timer_id) {
        for (TimedActivity *activity : m_game->running_activities()) {
            if (activity->isActive()) {
                activity->update_ui();
            }
        }
    }
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
