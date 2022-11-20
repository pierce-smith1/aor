#include "gamewindow.h"
#include "I."
#include "itemslot.h"
#include "externalslot.h"
#include "effectslot.h"
#include "cheatconsole.h"
#include "items.h"
#include "qmutex.h"

GameTimers::GameTimers(LKGameWindow *game)
    : game(game) { }

void GameTimers::timerEvent(QTimerEvent *event) {
    if (event->timerId() == activity_timer) {
        game->progress_activity(ACTIVITY_TICK_RATE_MS);
    }
}

LKGameWindow::LKGameWindow()
    : window(), timers(this)
{
    window.setupUi(this);

    connect(window.smith_button, &QPushButton::clicked, [&]() { start_activity({ Smithing, 50000 }); });
    connect(window.forage_button, &QPushButton::clicked, [&]() { start_activity({ Foraging, 50000 }); });
    connect(window.mine_button, &QPushButton::clicked, [&]() { start_activity({ Mining, 50000 }); });
    connect(window.pray_button, &QPushButton::clicked, [&]() { start_activity({ Praying, 50000 }); });

    ItemSlot::insert_inventory_slots(*this);
    ExternalSlot::insert_external_slots(*this);
    EffectSlot::insert_effect_slots(*this);
    ToolSlot::insert_tool_slot(*this);
    PortraitSlot::insert_portrait_slot(*this);

    notify(Discovery, "The Sun breaks on a new adventure.");
}

void LKGameWindow::register_slot_name(const std::string &slot_name) {
    slot_names.push_back(slot_name);
}

void LKGameWindow::mutate_state(std::function<void(State &)> action) {
    QMutexLocker lock(&mutex);

    action(character);
    refresh_ui();
}

void LKGameWindow::notify(NotificationType type, const std::string &message) {
    window.event_list->addItem(new GameNotification(type, message));
}

void LKGameWindow::start_activity(const CharacterActivity &activity) {
    QMutexLocker lock(&mutex);

    if (activity.action != Nothing) {
        lock_ui();
        timers.activity_timer_id = timers.startTimer(ACTIVITY_TICK_RATE_MS);
    }

    character.activity = activity;
}

void LKGameWindow::progress_activity(std::int64_t by_ms) {
    QMutexLocker lock(&mutex);

    character.activity.ms_left -= by_ms;

    if (character.activity.ms_left < 0) {
        complete_activity();
    }

    refresh_ui();
}

void LKGameWindow::refresh_ui() {
    QMutexLocker lock(&mutex);

    for (const std::string &slot_name : slot_names) {
        ItemSlot *slot = findChild<ItemSlot *>(QString::fromStdString(slot_name));
        slot->refresh_pixmap();
    }

    window.energy_bar->setValue(character.energy);
    window.energy_bar->setMaximum(character.get_max_energy());

    window.activity_time_bar->setValue(character.activity.ms_total - character.activity.ms_left);
    window.activity_time_bar->setMaximum(character.activity.ms_total);
}

bool LKGameWindow::activity_ongoing() {
    QMutexLocker lock(&mutex);

    return character.activity.action != Nothing;
}

void LKGameWindow::complete_activity() {
    QMutexLocker lock(&mutex);

    switch (character.activity.action) {
        case Smithing: {
            notify(ActionComplete, "You have finished smithing.");
        }
        default: {
            qFatal("What the fuck?");
        }
    }

    killTimer(timers.activity_timer_id);
    timers.activity_timer_id = 0;

    unlock_ui();
}

std::vector<QPushButton *> LKGameWindow::get_activity_buttons() {
    return {
        window.smith_button,
        window.forage_button,
        window.mine_button,
        window.pray_button,
    };
}

const std::vector<std::string> &LKGameWindow::get_item_slot_names() {
    return slot_names;
}

void LKGameWindow::lock_ui() {
    for (QPushButton *button : get_activity_buttons()) {
        button->setEnabled(false);
    }
}

void LKGameWindow::unlock_ui() {
    for (QPushButton *button : get_activity_buttons()) {
        button->setEnabled(true);
    }
}
