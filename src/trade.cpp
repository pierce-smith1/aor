#include "trade.h"

void DoughbyteConnection::checkin(Game &game) {
    QByteArray *data = new QByteArray;
    ByteArrayIO::write_byte(*data, API_VERSION);
    ByteArrayIO::write_long(*data, game.game_id());
    ByteArrayIO::write_byte(*data, REQ_CHECKIN);

    QTcpSocket *socket = new QTcpSocket;
    socket->connectToHost(remote_url, AOW_PORT);

    QObject::connect(socket, &QTcpSocket::connected, [=]() {
        qDebug("Checkin socket writing: %s", data->toHex().toStdString().c_str());
        socket->write(*data);
    });

    QObject::connect(socket, &QTcpSocket::readyRead, [=, &game]() {
        qDebug("Checkin socket: Remote data ready for reading (%lld bytes)", socket->bytesAvailable());
        QByteArray data = socket->readAll();
        if (data.length() == 0) {
            qWarning("Checkin returned no data");
        } if (data.length() == 1 && data[0] == RES_NOTHING_TO_DO) {
            qDebug("Checkin returned nothing to do");
        } else if (data[0] == RES_OFFER) {
            uint16_t offering_key = ByteArrayIO::read_short(data, 1);
            qDebug("Receiving offering with key %d", offering_key);

            int bytes_read = 3;
            while (bytes_read < socket->bytesAvailable()) {
                auto pair = ByteArrayIO::read_item(data, bytes_read);
                game.inventory().add_item(pair.first);
                bytes_read += pair.second;
            }
        }
    });

    QObject::connect(socket, &QTcpSocket::disconnected, [=]() {
        qDebug("Checkin socket closing");
        delete data;
        socket->deleteLater();
    });
}

void DoughbyteConnection::offer(Game &game, const std::vector<Item> &items, ItemCode key) {
    QByteArray *data = new QByteArray;
    ByteArrayIO::write_byte(*data, API_VERSION);
    ByteArrayIO::write_long(*data, game.game_id());
    ByteArrayIO::write_byte(*data, REQ_OFFER);
    ByteArrayIO::write_short(*data, key);
    ByteArrayIO::write_long(*data, game.game_id());
    ByteArrayIO::write_short(*data, items.size());
    for (const Item &item : items) {
        ByteArrayIO::write_item(*data, item);
    }

    QTcpSocket *socket = new QTcpSocket;
    socket->connectToHost(remote_url, AOW_PORT);

    QObject::connect(socket, &QTcpSocket::connected, [=]() {
        qDebug("Offer socket writing: %s", data->toHex().toStdString().c_str());
        socket->write(*data);
    });

    QObject::connect(socket, &QTcpSocket::readyRead, [=, &game]() {
        qDebug("Offer socket: Remote data ready for reading (%lld bytes)", socket->bytesAvailable());
        QByteArray data = socket->readAll();
        if (data.length() == 0) {
            qWarning("Offer returned no data");
        } if (data.length() == 1 && data[0] == RES_NOTHING_TO_DO) {
            qDebug("Offer returned nothing to do");
        } else if (data[0] == RES_OFFER) {
            uint16_t offering_key = ByteArrayIO::read_short(data, 1);
            qDebug("Receiving offering with key %d", offering_key);

            int bytes_read = 3;
            while (bytes_read < socket->bytesAvailable()) {
                auto pair = ByteArrayIO::read_item(data, bytes_read);
                game.inventory().add_item(pair.first);
                bytes_read += pair.second;
            }
        }
    });

    QObject::connect(socket, &QTcpSocket::disconnected, [=]() {
        qDebug("Offer socket closing");
        delete data;
        socket->deleteLater();
    });
}
