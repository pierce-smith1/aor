#pragma once

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

    void register_slot_name(const std::string &slot_name);

    template<typename T> T read_state(std::function<T(const State &)> action) {
        QMutexLocker lock(&mutex);
        return action(character);
    }
    void mutate_state(std::function<void(State &)> action);

    void notify(NotificationType type, const std::string &message);

    void start_activity(const CharacterActivity &activity);
    void progress_activity(std::int64_t by_ms);
    void refresh_ui();
    bool activity_ongoing();
    void complete_activity();

    std::vector<QPushButton *> get_activity_buttons();
    const std::vector<std::string> &get_item_slot_names();

    Ui::LKMainWindow window;
    Tooltip item_tooltip;
private:
    void lock_ui();
    void unlock_ui();

    GameTimers timers;
    QRecursiveMutex mutex;
    State character;
    std::vector<std::string> slot_names;
};
