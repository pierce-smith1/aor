// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QtGlobal>
#include <QIODevice>

#include "types.h"

namespace IO {
    struct RetryException {};

    AorUInt read_uint(QIODevice *dev);
    char read_byte(QIODevice *dev);
    QString read_string(QIODevice *dev);

    void write_uint(QIODevice *dev, AorUInt n);
    void write_byte(QIODevice *dev, char n);
    void write_string(QIODevice *dev, const QString &n);
};
