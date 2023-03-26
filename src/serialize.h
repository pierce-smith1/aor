#pragma once

#include "io.h"

#include <type_traits>

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
