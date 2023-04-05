// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <type_traits>
#include <vector>
#include <set>

#include "io.h"
#include "types.h"

class Serializable {};

namespace Serialize {
    template <typename G, std::enable_if_t<std::is_base_of<Serializable, G>::value, bool> = true> void serialize(QIODevice *dev, const G &game_component);
    template <typename C, typename = typename C::value_type> void serialize(QIODevice *dev, const C &container);

    inline void serialize(QIODevice *dev, AorUInt i) {
        IO::write_uint(dev, i);
    }

    inline void serialize(QIODevice *dev, AorInt i) {
        IO::write_uint(dev, i);
    }

    inline void serialize(QIODevice *dev, long unsigned i) {
        IO::write_uint(dev, i);
    }

    inline void serialize(QIODevice *dev, bool b) {
        IO::write_byte(dev, b);
    }

    inline void serialize(QIODevice *dev, const QString &s) {
        IO::write_string(dev, s);
    }

    template <typename F, typename S> void serialize(QIODevice *dev, const std::pair<F, S> &pair) {
        serialize(dev, pair.first);
        serialize(dev, pair.second);
    }

    template <typename C, typename> void serialize(QIODevice *dev, const C &container) {
        serialize(dev, (AorUInt) container.size());
        for (typename C::value_type item : container) {
            serialize(dev, item);
        }
    }

    template <typename G, std::enable_if_t<std::is_base_of<Serializable, G>::value, bool>> void serialize(QIODevice *dev, const G &game_component) {
        game_component.serialize(dev);
    }

    template <typename T, std::size_t N> void deserialize(QIODevice *dev, std::array<T, N> &container);
    template <typename G, std::enable_if_t<std::is_base_of<Serializable, G>::value, bool> = true> void deserialize(QIODevice *dev, G &component);

    inline void deserialize(QIODevice *dev, AorUInt &i) {
        i = IO::read_uint(dev);
    }

    inline void deserialize(QIODevice *dev, AorInt &i) {
        i = IO::read_uint(dev);
    }

    inline void deserialize(QIODevice *dev, IntWrapper<AorUInt> &i) {
        i = IO::read_uint(dev);
    }

    inline void deserialize(QIODevice *dev, bool &b) {
        b = IO::read_byte(dev);
    }

    inline void deserialize(QIODevice *dev, QString &s) {
        s = IO::read_string(dev);
    }

    template <typename E, std::enable_if_t<std::is_enum<E>::value, bool> = true> void deserialize(QIODevice *dev, E &e) {
        e = (E) IO::read_uint(dev);
    }

    template <typename F, typename S> void deserialize(QIODevice *dev, std::pair<F, S> &pair) {
        deserialize(dev, pair.first);
        deserialize(dev, pair.second);
    }

    template <typename T, std::size_t N> void deserialize(QIODevice *dev, std::array<T, N> &container) {
        AorUInt _size;
        deserialize(dev, _size);
        for (AorUInt i = 0; i < N; i++) {
            deserialize(dev, container[i]);
        }
    }

    template <typename T> void deserialize(QIODevice *dev, std::vector<T> &container) {
        AorUInt size;
        deserialize(dev, size);
        for (AorUInt i = 0; i < size; i++) {
            T item;
            deserialize(dev, item);
            container.push_back(item);
        }
    }

    template <typename C, typename = typename C::value_type> void deserialize(QIODevice *dev, C &container) {
        std::vector<typename C::value_type> items;
        deserialize(dev, items);
        container = C(items.begin(), items.end());
    }

    template <typename K, typename V> void deserialize(QIODevice *dev, std::map<K, V> &map) {
        std::vector<std::pair<K, V>> entries;
        deserialize(dev, entries);
        map = std::map<K, V>(entries.begin(), entries.end());
    }

    template <typename G, std::enable_if_t<std::is_base_of<Serializable, G>::value, bool>> void deserialize(QIODevice *dev, G &component) {
        component.deserialize(dev);
    }
}
