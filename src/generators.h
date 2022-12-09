#pragma once

#include <vector>
#include <utility>

#include <QtGlobal>
#include <QColor>
#include <QTime>
#include <QRandomGenerator>

#include "items.h"
#include "colors.h"

class Character;

namespace Generators {
    using Cluster = std::pair<QString, double>;

    QRandomGenerator *rng();

    template<typename T> T sample_with_weights(const std::vector<std::pair<T, double>> &weights) {
        if (weights.empty()) {
            qFatal("Tried to sample from an empty list");
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

    QString yokin_name(int length = 5);
    QString tribe_name();
    ItemId item_id();
    Color color();
    GameId game_id();
}
