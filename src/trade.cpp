#include "trade.h"
#include "gamewindow.h"

const QString DoughbyteConnection::remote_http_url("http://localhost:10241");
const QString DoughbyteConnection::remote_tcp_url("localhost");

DoughbyteConnection::DoughbyteConnection(LKGameWindow *game_window)
    : m_game_window(game_window)
{
    identify();
}

bool DoughbyteConnection::is_connected() {
    return m_is_connected;
}

void DoughbyteConnection::connect_to_ping_server() {
    QTcpSocket *connection = new QTcpSocket(m_game_window);
    connection->connectToHost(remote_tcp_url, 10242);

    QObject::connect(connection, &QTcpSocket::connected, [=]() {
        qDebug("Found ping server");
        m_is_connected = true;
        m_game_window->refresh_ui_buttons();
    });

    QObject::connect(connection, &QTcpSocket::disconnected, [=]() {
        qWarning("!!! Lost the ping server! PANIC!");
        m_is_connected = false;
        connection->deleteLater();
    });

    QObject::connect(connection, &QTcpSocket::readyRead, [=]() {
        for (int i = 0; i < connection->bytesAvailable(); i++) {
            char c;
            connection->getChar(&c);
            switch (c) {
                case MT_IGNORE: {
                    break;
                }
                case MT_UPDATE: {
                    update();
                    update_identities();
                    break;
                }
                case MT_EXECUTE: {
                    execute_trade();
                    break;
                }
                case MT_IDENTIFY: {
                    identify();
                    break;
                }
                case MT_UPDATEAGREEMENTS: {
                    update_agreements();
                    break;
                }
            }
        }
    });
}

void DoughbyteConnection::set_offering(int n, const Item &item) {
    http_request(
        [](QNetworkReply *) {
            qDebug("offeradd completed");
        },
        QString("%1;%2;%3;%4")
            .arg(m_game_window->game().game_id(), 0, 16)
            .arg(n, 0, 16)
            .arg(item.code, 0, 16)
            .arg(item.uses_left, 0, 16).toUtf8(),
        "/offeradd/"
    );
}

void DoughbyteConnection::update() {
    http_request(
        [this](QNetworkReply *reply) {
            qDebug("Update request completed");

            QString data = reply->readAll();

            QStringList tribe_sections = data.split("#");
            tribe_sections.takeFirst();

            for (const QString &tribe : tribe_sections) {
                GameId tribe_id = tribe.split("+")[0].toLongLong(nullptr, 16);

                QStringList offerings = tribe.split(":");
                offerings.takeFirst();

                for (int i = 0; i < offerings.size(); i++) {
                    QString offering = offerings[i];

                    ItemCode code = offering.split(";")[0].toShort(nullptr, 16);
                    unsigned char uses = offering.split(";")[1].toShort(nullptr, 16);
                    Item foreign_item(code);
                    foreign_item.uses_left = uses;

                    TradeState &this_trade = m_game_window->game().tribes()[tribe_id];
                    if (this_trade.offer[i].code != foreign_item.code) {
                        if (tribe_id == m_game_window->selected_tribe_id()) {
                            m_game_window->selected_char().accepting_trade() = false;
                            unaccept();
                        }
                        m_game_window->game().tribes()[tribe_id].offer[i] = foreign_item;
                        m_game_window->refresh_slots();
                    }
                }
            }

            reply->deleteLater();
        },
        "",
        "/update/"
    );
}

void DoughbyteConnection::update_identities() {
    http_request(
        [this](QNetworkReply *reply) {
            qDebug("Update identities request completed");

            QComboBox *tribes_box = m_game_window->window().trade_partner_combobox;
            QString data = reply->readAll();

            QStringList sections = data.split(":");
            sections.takeFirst();

            std::set<GameId> present_ids {};
            for (int i = 0; i < tribes_box->count(); i++) {
                present_ids.insert(tribes_box->itemData(i).toLongLong());
            }

            m_game_window->window().trade_partner_combobox->clear();
            for (QString section : sections) {
                QStringList parts = section.split(";");
                GameId id = parts[0].toLong(nullptr, 16);
                if (id != m_game_window->game().game_id() && present_ids.find(id) == end(present_ids)) {
                    m_game_window->window().trade_partner_combobox->insertItem(0, parts[1], QVariant::fromValue(id));
                } else if (id != m_game_window->game().game_id()) {
                    present_ids.erase(id);
                }
            }

            for (GameId missing_id : present_ids) {
                int index = tribes_box->findData(QVariant::fromValue(missing_id));
                tribes_box->removeItem(index);
            }

            reply->deleteLater();
        },
        "",
        "/updateidentities/"
    );
}

void DoughbyteConnection::update_agreements() {
    http_request(
        [this](QNetworkReply *reply) {
            qDebug("update agreements completed");

            QString data = reply->readAll();

            QStringList sections = data.split(":");
            sections.takeFirst();

            for (auto &pair : m_game_window->game().tribes()) {
                pair.second.remote_accepted = false;
            }

            for (QString section : sections) {
                QStringList parts = section.split(";");
                GameId accepting_id = parts[0].toLong(nullptr, 16);
                GameId accepted_id = parts[1].toLong(nullptr, 16);

                if (accepted_id == m_game_window->game().game_id()) {
                    m_game_window->game().tribes().at(accepting_id).remote_accepted = true;
                }
            }

            m_game_window->refresh_trade_ui();
            reply->deleteLater();
        },
        "",
        "/agreements/"
    );
}

void DoughbyteConnection::accept() {
    http_request(
        [](QNetworkReply *) {
            qDebug("accept completed");
        },
        QString("%1;%2")
            .arg(m_game_window->game().game_id(), 0, 16)
            .arg(m_game_window->selected_tribe_id(), 0, 16).toUtf8(),
        "/accept/"
    );
}

void DoughbyteConnection::unaccept() {
    http_request(
        [](QNetworkReply *) {
            qDebug("unaccept completed");
        },
        QString("%1").arg(m_game_window->game().game_id(), 0, 16),
        "/unaccept/"
    );
}

void DoughbyteConnection::execute_trade() {
    qDebug("trade...");
}

void DoughbyteConnection::identify() {
    http_request(
        [](QNetworkReply *) {
            qDebug("identify completed");
        },
        QString("%1;%2")
            .arg(m_game_window->game().game_id(), 0, 16)
            .arg(m_game_window->game().tribe_name()).toUtf8(),
        "/identify/"
    );
}

void DoughbyteConnection::http_request(std::function<void(QNetworkReply *)> on_finish, QString payload, QString path) {
    QNetworkAccessManager *client = new QNetworkAccessManager(m_game_window);
    QObject::connect(client, &QNetworkAccessManager::finished, on_finish);

    QNetworkRequest request = QNetworkRequest(QUrl(remote_http_url + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "x-rhodon");

    if (payload.isEmpty()) {
        client->get(request);
    } else {
        client->post(request, payload.toUtf8());
    }
}
