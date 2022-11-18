#pragma once

#include <vector>
#include <utility>

#include <QtGlobal>
#include <QTime>
#include <QRandomGenerator>

namespace Generators {
    using Cluster = std::pair<std::string, double>;

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

    std::string generate_yokin_name(size_t length = 5);
}
