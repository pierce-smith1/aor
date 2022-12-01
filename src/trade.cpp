#include "trade.h"

QUrl DoughbyteConnection::remote_url("doughbyte.com");

void DoughbyteConnection::heartbeat() {
    QByteArray *data = new QByteArray;
    data->resize(1);
    (*data)[0] = API_VERSION;

    QBuffer *buffer = new QBuffer(data);
    client.post(QNetworkRequest(remote_url), buffer);
}
