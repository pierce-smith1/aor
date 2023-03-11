#pragma once

#include <QtCore>

using Int = quint64;

template <typename IntType> struct IntWrapper {
    IntWrapper() : n(0) {}
    IntWrapper(IntType n) : n(n) {}

    bool operator == (const IntWrapper &other) { return n == other.n; }
    bool operator != (const IntWrapper &other) { return n != other.n; }
    bool operator < (const IntWrapper &other) { return n < other.n; }
    bool operator <= (const IntWrapper &other) { return n <= other.n; }
    bool operator > (const IntWrapper &other) { return n > other.n; }
    bool operator >= (const IntWrapper &other) { return n >= other.n; };

    operator quint64() const { return n; }

    IntType n;
};

#define ID_TYPE(i, t) struct i : public IntWrapper<t> { i () : IntWrapper() {} i (t n) : IntWrapper(n) {} }

ID_TYPE(ItemId, quint64);
ID_TYPE(CharacterId, quint16);
ID_TYPE(GameId, quint64);
ID_TYPE(ActivityId, quint64);

