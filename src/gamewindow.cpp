#include "gamewindow.h"
#include "inventory_ui.h"
#include "cheatconsole.h"
#include "items.h"

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
}

void LKGameWindow::register_slot_name(const std::string &slot_name) {
    slot_names.push_back(slot_name);
}

void LKGameWindow::mutate_state(std::function<void(State &)> action) {
    QMutexLocker lock(&mutex);

    action(character);
    refresh_item_slots();
}

void LKGameWindow::refresh_item_slots() {
    QMutexLocker lock(&mutex);

    for (const std::string &slot_name : slot_names) {
        ItemSlot *slot = findChild<ItemSlot *>(QString::fromStdString(slot_name));
        slot->refresh_pixmap();
    }
}

void LKGameWindow::start_activity(const CharacterActivity &activity) {
    QMutexLocker lock(&mutex);

    lock_ui();
    character.activity = activity;
    notify_activity();

    timers.activity_timer = timers.startTimer(ACTIVITY_TICK_RATE_MS);
}

void LKGameWindow::progress_activity(std::int64_t by_ms) {
    QMutexLocker lock(&mutex);

    character.activity.ms_left -= by_ms;
    notify_activity();

    if (character.activity.ms_left <= 0) {
        start_activity({ Nothing, 0 });
        unlock_ui();
        killTimer(timers.activity_timer);
    }
}

void LKGameWindow::notify_activity() {
    QMutexLocker lock(&mutex);

    auto notify = [&](const std::string &action_name) {
        window.statusbar->showMessage(QString("Currently %1: %2 minutes left...")
            .arg(QString::fromStdString(action_name))
            .arg(character.activity.ms_left / 60000.0));
    };

    switch (character.activity.action) {
        case Nothing: {
            window.statusbar->showMessage(QString("Doing nothing"));
            break;
        }
        case Smithing: notify("smithing a cool new tool"); break;
        case Eating: notify("eating a yummy snack"); break;
        case Foraging: notify("foraging for eats"); break;
        case Mining: notify("looking for shiny metals"); break;
        case Trading: notify("venturing for a trade"); break;
        case Praying: notify("dreaming of the gods"); break;
        case Sleeping: notify("fast asleep"); break;
        default: notify("???"); break;
    }
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
