#include "bytearrayio.h"

void ByteArrayIO::write_byte(QByteArray &array, char n) {
    array.append(n);
}

void ByteArrayIO::write_short(QByteArray &array, std::uint16_t n) {
    array.append((n & 0xff00) >> 8);
    array.append(n & 0xff);
}

void ByteArrayIO::write_long(QByteArray &array, std::uint64_t n) {
    array.append((n & 0xff00000000000000) >> 56);
    array.append((n & 0xff000000000000) >> 48);
    array.append((n & 0xff0000000000) >> 40);
    array.append((n & 0xff00000000) >> 32);
    array.append((n & 0xff000000) >> 24);
    array.append((n & 0xff0000) >> 16);
    array.append((n & 0xff00) >> 8);
    array.append(n & 0xff);
}

char ByteArrayIO::read_byte(const QByteArray &array, size_t from) {
    return array.at(from);
}

std::uint16_t ByteArrayIO::read_short(const QByteArray &array, size_t from) {
    return array.at(from)
        | (array.at(from + 1) << 8);
}

std::uint64_t ByteArrayIO::read_long(const QByteArray &array, size_t from) {
    return array.at(from)
        | ((std::uint64_t) array.at(from + 1) << 8)
        | ((std::uint64_t) array.at(from + 2) << 16)
        | ((std::uint64_t) array.at(from + 3) << 24)
        | ((std::uint64_t) array.at(from + 4) << 32)
        | ((std::uint64_t) array.at(from + 5) << 40)
        | ((std::uint64_t) array.at(from + 6) << 48)
        | ((std::uint64_t) array.at(from + 7) << 56);
}
