#pragma once

#include <algorithm>
#include <memory>

#include <QObject>
#include <QRecursiveMutex>
#include <QIODevice>

class LKGameWindow;

#include "items.h"
#include "../ui_main.h"
#include "gamenotification.h"
#include "character.h"
#include "game.h"
#include "trade.h"

class ItemSlot;
class RecipieBox;
class Encyclopedia;
class Tooltip;

using ActivityTimers = std::map<CharacterId, int>;

static const QString SAVE_FILE_NAME = "save.rho";

class LKGameWindow : public QMainWindow {
    Q_OBJECT

public:
    static void instantiate_singleton();
    LKGameWindow(); // Although this class is a singleton, we leave its
                    // constructor public for use with std::allocator.

    bool initialized();
    Game &game();
    Ui::LKMainWindow &window();
    Tooltip *&tooltip();
    CharacterId &selected_char_id();
    GameId &selected_tribe_id();
    DoughbyteConnection &connection();
    Encyclopedia *&encyclopedia();

    Character &selected_char();
    void swap_char(CharacterId char_id);

    void register_slot(ItemSlot *slot);

    void notify(NotificationType type, const QString &message);

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
    void closeEvent(QCloseEvent *event) override;

private:
    static LKGameWindow *the_game_window;

    bool m_initialized = false;
    CharacterId m_selected_char_id = 0;
    Ui::LKMainWindow m_window;
    Tooltip *m_item_tooltip;
    Game m_game;
    GameId m_selected_tribe_id = NOBODY;
    std::vector<ItemSlot *> m_slots;
    DoughbyteConnection m_connection;
    QFile m_save_file;
    Encyclopedia *m_encyclopedia;

    friend LKGameWindow *gw();
};

LKGameWindow *gw();
