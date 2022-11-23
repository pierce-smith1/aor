#pragma once

#include <algorithm>

#include <QObject>
#include <QRecursiveMutex>

class LKGameWindow;

#include "items.h"
#include "state.h"
#include "../ui_main.h"
#include "gamenotification.h"
#include "tooltip.h"

class GameTimers : public QObject {
    Q_OBJECT;
public:
    GameTimers(LKGameWindow *game);
    LKGameWindow *game;
    int activity_timer_id;
protected:
    void timerEvent(QTimerEvent *event) override;
};

class LKGameWindow : public QMainWindow {
public:
    LKGameWindow();

    void register_slot_name(const QString &slot_name);

    void notify(NotificationType type, const QString &message);

    void start_activity(ItemDomain type);
    void start_activity(const CharacterActivity &activity);
    void progress_activity(std::int64_t by_ms);
    void refresh_ui();
    void refresh_ui_bars();
    void refresh_ui_buttons();
    bool activity_ongoing();
    void complete_activity();

    const std::map<ItemDomain, QPushButton *> get_activity_buttons();
    const std::vector<QString> &get_item_slot_names();

    Ui::LKMainWindow window;
    Tooltip item_tooltip;
    State character;
private:
    void lock_ui();
    void unlock_ui();

    GameTimers timers;
    std::vector<QString> slot_names;
    double visual_energy;
    double visual_morale;
};
