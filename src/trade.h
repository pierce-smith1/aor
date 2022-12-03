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

class LKGameWindow;

const static char API_VERSION = 1;

const static char MT_IGNORE = '\0';
const static char MT_UPDATE = 'u';
const static char MT_UPDATEAGREEMENTS = 'a';
const static char MT_EXECUTE = 'e';
const static char MT_IDENTIFY = 'i';

class DoughbyteConnection {
public:
    const static QString remote_http_url;
    const static QString remote_tcp_url;

    DoughbyteConnection(LKGameWindow *game_window);

    bool is_connected();

    void connect_to_ping_server();
    void set_offering(int n, const Item &item);
    void update();
    void update_identities();
    void update_agreements();
    void accept();
    void unaccept();
    void execute_trade();
    void identify();

private:
    void http_request(std::function<void(QNetworkReply *)> on_finish, QString payload, QString path);

    LKGameWindow *m_game_window;
    bool m_is_connected = false;
};
