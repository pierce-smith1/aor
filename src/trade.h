#pragma once

#include <QString>
#include <QTcpSocket>

#include "character.h"
#include "items.h"
#include "io.h"
#include "game.h"

class LKGameWindow;

const static char API_VERSION = 1;

const static char MT_IGNORE = 0;
const static char MT_OFFERCHANGED = 'o';
const static char MT_AGREEMENTCHANGED = 'g';
const static char MT_TRIBEAVAILABILITYCHANGED = 't';
const static char MT_WANTGAMESTATE = 'w';
const static char MT_MYINFO = 'i';
const static char MT_EXECUTETRADE = 'e';

class DoughbyteConnection {
public:
    DoughbyteConnection(LKGameWindow *game_window);

    bool is_connected();

    void offer_changed(const Item &item, int n);
    void agreement_changed(GameId partner_id, bool accepting);
    void availability_changed(bool available);
    void send_info();
    void send_info(GameId to, const std::array<Item, TRADE_SLOTS> &items, bool accepting);
    void want_game_state();
    void execute_trade();

    void update_offers();
    void update_offers(GameId tribe_id, ItemCode code, char uses, int n);
    void update_agreements();
    void update_agreements(GameId tribe_id, bool agrees);
    void update_availability();
    void update_availability(GameId tribe_id, const QString &tribe_name, bool available);
    void update_all();
    void notify_trade(GameId tribe_id);

private:
    QTcpSocket m_socket;
    LKGameWindow *m_game_window;
    bool m_is_connected = false;
};
