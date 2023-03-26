#pragma once

#include <type_traits>

#include "io.h"
#include "types.h"

class Serializable {};

namespace Serialize {
    template <typename Container, typename = typename Container::value_type> void serialize(QIODevice *dev, const Container &c);
    template <typename Component, std::enable_if_t<std::is_base_of<Serializable, Component>::value, bool> = true> void serialize(QIODevice *dev, const Component &s);

    template <
        typename Integer,
        std::enable_if_t<std::is_integral<Integer>::value, bool> = true
    > void serialize(QIODevice *dev, const Integer i) {
        IO::write_uint(dev, i);
    }

    template <
        typename Integer,
        std::enable_if_t<std::is_base_of<IntWrapper<AorUInt>, Integer>::value, bool> = true
    > void serialize(QIODevice *dev, const Integer i) {
        IO::write_uint(dev, i);
    }

    template <
        typename Char,
        std::enable_if_t<std::is_same<Char, QChar>::value, bool> = true
    > void serialize(QIODevice *dev, const Char &c) {
        IO::write_byte(dev, c.toLatin1());
    }

    template <
        typename Enum,
        std::enable_if_t<std::is_enum<Enum>::value, bool> = true
    > void serialize(QIODevice *dev, const Enum i) {
        IO::write_uint(dev, i);
    }

    template <
        typename Pointer,
        std::enable_if_t<std::is_pointer<Pointer>::value, bool> = true
    > void serialize(QIODevice *dev, const Pointer p) {
        serialize(dev, *p);
    }

    template <
        typename Pair,
        typename = typename Pair::first_type,
        typename = typename Pair::second_type
    > void serialize(QIODevice *dev, const Pair &p) {
        serialize(dev, p.first);
        serialize(dev, p.second);
    }

    template <
        typename Container,
        typename
    > void serialize(QIODevice *dev, const Container &c) {
        IO::write_uint(dev, c.size());
        for (const typename Container::value_type &i : c) {
            serialize(dev, i);
        }
    }

    template <
        typename Component,
        std::enable_if_t<std::is_base_of<Serializable, Component>::value, bool>
    > void serialize(QIODevice *dev, const Component &s) {
        s.serialize(dev);
    }

    template <typename Container, typename = typename Container::value_type> void deserialize(QIODevice *dev, Container *c);
    template <typename Component, std::enable_if_t<std::is_base_of<Serializable, Component>::value, bool> = true> void deserialize(QIODevice *dev, Component *s);

    template <
        typename Integer,
        std::enable_if_t<std::is_integral<Integer>::value, bool> = true
    > void deserialize(QIODevice *dev, Integer *i) {
        *i = IO::read_uint(dev);
    }

    template <
        typename Integer,
        std::enable_if_t<std::is_base_of<IntWrapper<AorUInt>, Integer>::value, bool> = true
    > void deserialize(QIODevice *dev, const Integer *i) {
        *i = IO::read_uint(dev);
    }

    template <
        typename Char,
        std::enable_if_t<std::is_same<Char, QChar>::value, bool> = true
    > void serialize(QIODevice *dev, Char *c) {
        *c = IO::read_byte(dev);
    }

    template <
        typename Enum,
        std::enable_if_t<std::is_enum<Enum>::value, bool> = true
    > void deserialize(QIODevice *dev, Enum *i) {
        *i = (Enum) IO::read_uint(dev);
    }

    template <
        typename Pair,
        typename = typename Pair::first_type,
        typename = typename Pair::second_type
    > void deserialize(QIODevice *dev, Pair *p) {
        deserialize(dev, p->first);
        deserialize(dev, p->second);
    }

    template <
        typename Container,
        typename
    > void deserialize(QIODevice *dev, Container *c) {
        AorUInt size;
        deserialize(dev, &size);

        std::vector<typename Container::value_type> items;
        for (AorUInt i = 0; i < size; i++) {
            typename Container::value_type t;
            deserialize(dev, &t);
            items.push_back(t);
        }

        std::copy(items.begin(), items.end(), std::back_inserter(*c));
    }

    template <
        typename Component,
        std::enable_if_t<std::is_base_of<Serializable, Component>::value, bool>
    > void deserialize(QIODevice *dev, Component *s) {
        s->deserialize(dev);
    }
}

/*
template <
    typename Integer,
    std::enable_if_t<std::is_integral<Integer>::value, bool> = true
> void serialize(QIODevice *dev, Integer i) {
    IO::write_uint(dev, i);
}

template <
    typename Pair
> void serialize(QIODevice *dev, Pair &p) {
    serialize(dev, p.first);
    serialize(dev, p.second);
}

template <
    typename Container,
    typename Container::Inner
> void serialize(QIODevice *dev, Container &c) {
    IO::write_uint(dev, c.size());
    for (typename Container::Inner i : c) {
        serialize(dev, i);
    }
}

template <
    typename Component,
    std::enable_if_t<std::is_base_of<Serializable, Component>::value, bool> = true
> void serialize(QIODevice *dev, Component &s) {
    s.serialize(dev);
}
*/
