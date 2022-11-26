#pragma once

#include <QString>
#include <QNetworkAccessManager>

#include "character.h"
#include "items.h"

struct TradeOffering {
    ItemCode key;
    std::vector<Item> items;
};

namespace Trade {
    static QString trade_server = "doughbyte.com";
    static QNetworkAccessManager net;

    void heartbeat();
    void send_offering(const TradeOffering &offering);
    void recieve_offering(Character &character, const TradeOffering &offering);
}
