#pragma once

#include <QtGlobal>
#include <QIODevice>

#include "items.h"

namespace IO {
    struct RetryException {};

    AorUInt read_uint(QIODevice *dev);
    char read_byte(QIODevice *dev);
    QString read_string(QIODevice *dev);
    Item read_item(QIODevice *dev);

    void write_uint(QIODevice *dev, AorUInt n);
    void write_byte(QIODevice *dev, char n);
    void write_string(QIODevice *dev, const QString &n);
    void write_item(QIODevice *dev, const Item &n);
};
