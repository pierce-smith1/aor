#pragma once

#include <QObject>
#include <QRecursiveMutex>

class LKGameWindow;

#include "items.h"
#include "state.h"
#include "../ui_main.h"
#include "tooltip.h"

class GameTimers : public QObject {
    Q_OBJECT;
public:
    GameTimers(LKGameWindow *game);
    LKGameWindow *game;
    int activity_timer;
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

    void start_activity(const CharacterActivity &activity);
    void progress_activity(std::int64_t by_ms);
    void notify_activity();

    std::vector<QPushButton *> get_activity_buttons();
    const std::vector<std::string> &get_item_slot_names();

    Ui::LKMainWindow window;
    Tooltip item_tooltip;
private:
    void refresh_item_slots();
    void lock_ui();
    void unlock_ui();

    GameTimers timers;
    QRecursiveMutex mutex;
    State character;
    std::vector<std::string> slot_names;
};
