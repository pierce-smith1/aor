#pragma once

#include <algorithm>

#include <QObject>
#include <QRecursiveMutex>
#include <QIODevice>

class LKGameWindow;

#include "items.h"
#include "../ui_main.h"
#include "gamenotification.h"
#include "tooltip.h"
#include "character.h"
#include "game.h"
#include "explorerbutton.h"
#include "trade.h"

class ItemSlot;

using ActivityTimers = std::map<CharacterId, int>;

static const QString SAVE_FILE_NAME = "save.rho";

class LKGameWindow : public QMainWindow {
    Q_OBJECT

public:
    LKGameWindow();

    Game &game();
    Ui::LKMainWindow &window();
    Tooltip &tooltip();
    CharacterId &selected_char_id();
    GameId &selected_tribe_id();
    DoughbyteConnection &connection();

    Character &selected_char();
    void swap_char(CharacterId char_id);

    void register_slot(ItemSlot *slot);

    void notify(NotificationType type, const QString &message);

    void start_activity(CharacterId char_id, ItemDomain type);
    void start_activity(const CharacterActivity &activity);
    void start_activity(CharacterId char_id, const CharacterActivity &activity);
    void progress_activity(CharacterId char_id, std::int64_t by_ms);
    void complete_activity(CharacterId char_id);

    bool trade_ongoing(GameId tribe);

    void refresh_ui();
    void refresh_slots();
    void refresh_ui_bars();
    void refresh_ui_buttons();
    void refresh_trade_ui();

    const std::map<ItemDomain, QPushButton *> get_activity_buttons();
    const std::vector<ItemSlot *> &item_slots();
    const std::vector<ItemSlot *> item_slots(ItemDomain domain);

    void save();
    void load();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    Ui::LKMainWindow m_window;
    Tooltip m_item_tooltip;
    Game m_game;
    CharacterId m_selected_char_id = 0;
    GameId m_selected_tribe_id = NOBODY;
    ActivityTimers m_timers;
    std::vector<ItemSlot *> m_slots;
    DoughbyteConnection m_connection;
    QFile m_save_file;
};
