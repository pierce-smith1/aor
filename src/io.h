#pragma once

#include <QtGlobal>
#include <QIODevice>

#include "items.h"

namespace IO {
    struct RetryException {};

    quint64 read_long(QIODevice *dev);
    quint16 read_short(QIODevice *dev);
    char read_byte(QIODevice *dev);
    bool read_bool(QIODevice *dev);
    QString read_string(QIODevice *dev);
    Item read_item(QIODevice *dev);

    void write_long(QIODevice *dev, quint64 n);
    void write_short(QIODevice *dev, quint16 n);
    void write_byte(QIODevice *dev, char n);
    void write_bool(QIODevice *dev, bool n);
    void write_string(QIODevice *dev, const QString &n);
    void write_item(QIODevice *dev, const Item &n);
};
