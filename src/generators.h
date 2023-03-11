#pragma once

#include <vector>
#include <utility>

#include <QtGlobal>
#include <QColor>
#include <QTime>
#include <QRandomGenerator>

#include "colors.h"
#include "die.h"
#include "types.h"

class Character;

// WARNING: also exists in hooks.h, since it cannot include this file
template <typename T> using WeightedVector = std::vector<std::pair<T, double>>;

namespace Generators {
    using Cluster = std::pair<QString, double>;

    QRandomGenerator *rng();

    template <typename T> std::vector<T> without_weights(const WeightedVector<T> &vector) {
        std::vector<T> unweighted_vector;

        for (const auto &pair : vector) {
            unweighted_vector.push_back(pair.first);
        }

        return unweighted_vector;
    }

    template <typename T> WeightedVector<T> with_trivial_weights(const std::vector<T> &vector) {
        WeightedVector<T> weighted_vector;

        for (const auto &item : vector) {
            weighted_vector.push_back({ item, 1.0 });
        }

        return weighted_vector;
    }

    template <typename T> T sample_with_weights(const WeightedVector<T> &weights) {
        if (weights.empty()) {
            bugcheck(EmptyListSample);
        }

        double total_weight = 0.0;
        for (const std::pair<T, double> &pair : weights) {
            total_weight += pair.second;
        }

        int r = rng()->generate() % 10000;
        double running_weight = 0.0;
        for (const std::pair<T, double> &pair : weights) {
            running_weight += (pair.second / total_weight) * 10000;
            if (r < running_weight) {
                return pair.first;
            }
        }

        return weights.back().first;
    }

    QString yokin_name();
    QString tribe_name();
    ItemId item_id();
    Color color();
    GameId game_id();
    CharacterId char_id();
    ActivityId activity_id();
    bool percent_chance(int p);
}
