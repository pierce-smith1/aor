#pragma once

#include <QByteArray>
#include "items.h"

namespace ByteArrayIO {
    void write_byte(QByteArray &array, char n);
    void write_short(QByteArray &array, std::uint16_t n);
    void write_long(QByteArray &array, std::uint64_t n);

    void write_item(QByteArray &array, const Item &item);

    char read_byte(const QByteArray &array, size_t from);
    std::uint16_t read_short(const QByteArray &array, size_t from);
    std::uint64_t read_long(const QByteArray &array, size_t from);

    std::pair<Item, int> read_item(const QByteArray &array, size_t from);
}
