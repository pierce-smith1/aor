#pragma once

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QTcpSocket>

#include "character.h"
#include "items.h"
#include "bytearrayio.h"
#include "game.h"

const static char API_VERSION = 1;

const static char REQ_CHECKIN = 0;
const static char REQ_OFFER = 1;

const static char RES_NOTHING_TO_DO = 0;
const static char RES_OFFER = 1;
const static char RES_ERROR = 2;

const static int AOW_PORT = 10241;
const static int CHECKIN_INTERVAL_MS = 5000;

struct TradeOffering {
    ItemCode key;
    std::vector<Item> items;
};

namespace DoughbyteConnection {
    static QString remote_url("doughbyte.com");

    void checkin(Game &game);
    void offer(Game &game, const std::vector<Item> &items, ItemCode key);
};
