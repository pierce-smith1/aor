#pragma once

#include <QString>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QTcpServer>

#include "character.h"
#include "items.h"

const static char API_VERSION = 1;
const static int AOW_PORT = 10241;
const static int HEARTBEAT_INTERVAL_MS = 1000 * 20;

struct TradeOffering {
    ItemCode key;
    std::vector<Item> items;
};

class DoughbyteConnection {
public:
    static QUrl remote_url;

    DoughbyteConnection();

    void heartbeat();
    void recieve_offering(Character &character, const TradeOffering &offering);
private:
    void recieve();
    void on_recieve_hb_response();
    void on_recieve_offer();

    QNetworkAccessManager client;
};
