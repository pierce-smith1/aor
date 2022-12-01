#pragma once

#include <algorithm>

#include <QObject>
#include <QRecursiveMutex>

class LKGameWindow;

#include "items.h"
#include "../ui_main.h"
#include "gamenotification.h"
#include "tooltip.h"
#include "character.h"
#include "game.h"
#include "explorerbutton.h"
#include "trade.h"

using ActivityTimers = std::map<CharacterId, int>;

class LKGameWindow : public QMainWindow {
    Q_OBJECT

public:
    LKGameWindow();

    Game &game();
    Ui::LKMainWindow &window();
    Tooltip &tooltip();
    CharacterId &selected_char_id();

    Character &selected_char();
    void swap_char(CharacterId char_id);

    void register_slot_name(const QString &slot_name);

    void notify(NotificationType type, const QString &message);

    void start_activity(CharacterId char_id, ItemDomain type);
    void start_activity(const CharacterActivity &activity);
    void start_activity(CharacterId char_id, const CharacterActivity &activity);
    void progress_activity(CharacterId char_id, std::int64_t by_ms);
    void complete_activity(CharacterId char_id);

    void refresh_ui();
    void refresh_slots();
    void refresh_ui_bars();
    void refresh_ui_buttons();

    const std::map<ItemDomain, QPushButton *> get_activity_buttons();
    const std::vector<QString> &item_slot_names();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    Ui::LKMainWindow m_window;
    Tooltip m_item_tooltip;
    Game m_game;
    CharacterId m_selected_char_id {0};
    ActivityTimers m_timers;
    std::vector<QString> m_slot_names;
    int m_heartbeat_timer;
    DoughbyteConnection m_db_connection;
};
