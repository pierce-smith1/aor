#pragma once

#include <QObject>
#include <QRecursiveMutex>

class LKGameWindow;

#include "items.h"
#include "state.h"
#include "../ui_main.h"

class InventoryEventFilter : public QObject {
    Q_OBJECT;
public:
    InventoryEventFilter(LKGameWindow *game);
    LKGameWindow *game;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

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

    template<typename T> T read_state(std::function<T(const State &)> action) {
        QMutexLocker lock(&mutex);
        return action(character);
    }
    void mutate_state(std::function<void(State &)> action);

    void start_activity(const CharacterActivity &activity);
    void progress_activity(std::int64_t by_ms);
    void notify_activity();

    std::vector<QPushButton *> get_activity_buttons();

    Ui::LKMainWindow window;
private:
    void refresh_inventory();
    void lock_ui();
    void unlock_ui();

    GameTimers timers;
    QRecursiveMutex mutex;
    State character;
};
