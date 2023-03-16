#include "io.h"

AorUInt IO::read_uint(QIODevice *dev) {
    QByteArray data = dev->read(8);

    if (data.count() < 8) {
        dev->rollbackTransaction();
        throw RetryException();
    }

    AorUInt decoded = (((AorUInt) data[7]) & 0xff)
        | ((((AorUInt) data[6]) & 0xff) << 8)
        | ((((AorUInt) data[5]) & 0xff) << 16)
        | ((((AorUInt) data[4]) & 0xff) << 24)
        | ((((AorUInt) data[3]) & 0xff) << 32)
        | ((((AorUInt) data[2]) & 0xff) << 40)
        | ((((AorUInt) data[1]) & 0xff) << 48)
        | ((((AorUInt) data[0]) & 0xff) << 56);

    return decoded;
}

char IO::read_byte(QIODevice *dev) {
    char c;

    if (!dev->getChar(&c)) {
        dev->rollbackTransaction();
        throw RetryException();
    }

    return c;
}

QString IO::read_string(QIODevice *dev) {
    AorUInt size = read_uint(dev);
    QByteArray data = dev->read(size);

    if (static_cast<AorUInt>(data.count()) < size) {
        dev->rollbackTransaction();
        throw RetryException();
    }

    return data;
}

Item IO::read_item(QIODevice *dev) {
    Item item;

    item.code = read_uint(dev);
    item.id = read_uint(dev);
    item.uses_left = read_uint(dev);
    item.owning_action = read_uint(dev);

    AorUInt size = read_uint(dev);
    for (AorUInt i = 0; i < size; i++) {
        ItemProperty prop = (ItemProperty) read_uint(dev);
        item.instance_properties.map[prop] = read_uint(dev);
    }

    return item;
}

void IO::write_uint(QIODevice *dev, AorUInt n) {
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

void IO::write_byte(QIODevice *dev, char n) {
    char data[1];

    data[0] = n;

    dev->write(data, 1);
}

void IO::write_string(QIODevice *dev, const QString &n) {
    write_uint(dev, n.size());
    dev->write(n.toUtf8());
}

void IO::write_item(QIODevice *dev, const Item &n) {
    write_uint(dev, n.code);
    write_uint(dev, n.id);
    write_uint(dev, n.uses_left);
    write_uint(dev, n.owning_action);

    write_uint(dev, n.instance_properties.map.size());
    for (const auto &pair : n.instance_properties) {
        write_uint(dev, pair.first);
        write_uint(dev, pair.second);
    }
}
