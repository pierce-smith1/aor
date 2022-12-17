#include "io.h"

quint64 IO::read_long(QIODevice *dev) {
    QByteArray data = dev->read(8);

    if (data.count() < 8) {
        dev->rollbackTransaction();
        throw RetryException();
    }

    quint64 decoded = (((quint64) data[7]) & 0xff)
        | ((((quint64) data[6]) & 0xff) << 8)
        | ((((quint64) data[5]) & 0xff) << 16)
        | ((((quint64) data[4]) & 0xff) << 24)
        | ((((quint64) data[3]) & 0xff) << 32)
        | ((((quint64) data[2]) & 0xff) << 40)
        | ((((quint64) data[1]) & 0xff) << 48)
        | ((((quint64) data[0]) & 0xff) << 56);

    return decoded;
}

quint16 IO::read_short(QIODevice *dev) {
    QByteArray data = dev->read(2);

    if (data.count() < 2) {
        dev->rollbackTransaction();
        throw RetryException();
    }

    return data[1] | ((data[0]) << 8);
}

char IO::read_byte(QIODevice *dev) {
    char c;

    if (!dev->getChar(&c)) {
        dev->rollbackTransaction();
        throw RetryException();
    }

    return c;
}

bool IO::read_bool(QIODevice *dev) {
    return read_byte(dev) != 0;
}

QString IO::read_string(QIODevice *dev) {
    char size = read_byte(dev);
    QByteArray data = dev->read(size);

    if (data.count() < size) {
        dev->rollbackTransaction();
        throw RetryException();
    }

    return data;
}

Item IO::read_item(QIODevice *dev) {
    Item item;

    item.code = read_short(dev);
    item.id = read_long(dev);
    item.uses_left = read_byte(dev);
    item.intent = (ItemDomain) read_short(dev);

    quint16 size = read_short(dev);
    for (quint16 i = 0; i < size; i++) {
        ItemProperty prop = (ItemProperty) read_short(dev);
        item.instance_properties.map[prop] = IO::read_short(dev);
    }

    return item;
}

void IO::write_long(QIODevice *dev, quint64 n) {
    char data[8];

    data[7] = n & 0xff;
    data[6] = (n & 0xff00) >> 8;
    data[5] = (n & 0xff0000) >> 16;
    data[4] = (n & 0xff000000) >> 24;
    data[3] = (n & 0xff00000000) >> 32;
    data[2] = (n & 0xff0000000000) >> 40;
    data[1] = (n & 0xff000000000000) >> 48;
    data[0] = (n & 0xff00000000000000) >> 56;

    dev->write(data, 8);
}

void IO::write_short(QIODevice *dev, quint16 n) {
    char data[2];

    data[1] = n & 0xff;
    data[0] = (n & 0xff00) >> 8;

    dev->write(data, 2);
}

void IO::write_byte(QIODevice *dev, char n) {
    char data[1];

    data[0] = n;

    dev->write(data, 1);
}

void IO::write_bool(QIODevice *dev, bool n) {
    char data[1];

    data[0] = n ? 1 : 0;

    dev->write(data, 1);
}

void IO::write_string(QIODevice *dev, const QString &n) {
    write_byte(dev, n.size());
    dev->write(n.toUtf8());
}

void IO::write_item(QIODevice *dev, const Item &n) {
    write_short(dev, n.code);
    write_long(dev, n.id);
    write_byte(dev, n.uses_left);
    write_short(dev, n.intent);

    write_short(dev, n.instance_properties.map.size());
    for (const auto &pair : n.instance_properties) {
        write_short(dev, pair.first);
        write_short(dev, pair.second);
    }
}
