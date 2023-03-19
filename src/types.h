#pragma once

#include <QtCore>

using AorUInt = quint64;
using AorInt = qint64;

template <typename IntType> struct IntWrapper {
    IntWrapper() : n(0) {}
    IntWrapper(IntType n) : n(n) {}

    bool operator == (const IntWrapper &other) { return n == other.n; }
    bool operator != (const IntWrapper &other) { return n != other.n; }
    bool operator < (const IntWrapper &other) { return n < other.n; }
    bool operator <= (const IntWrapper &other) { return n <= other.n; }
    bool operator > (const IntWrapper &other) { return n > other.n; }
    bool operator >= (const IntWrapper &other) { return n >= other.n; }

    operator IntType() const { return n; }

    IntType n;
};

#define INT_TYPE(i, t) struct i : public IntWrapper<t> { i () : IntWrapper() {} i (t n) : IntWrapper(n) {} }

INT_TYPE(ItemId, AorUInt);
INT_TYPE(CharacterId, AorUInt);
INT_TYPE(GameId, AorUInt);
INT_TYPE(ActivityId, AorUInt);
INT_TYPE(LocationId, AorUInt);
INT_TYPE(ItemCode, AorUInt);
INT_TYPE(ItemType, AorUInt);
