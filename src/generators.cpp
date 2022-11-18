#include "generators.h"

QRandomGenerator *Generators::rng() {
    static QRandomGenerator *rng = new QRandomGenerator(QTime::currentTime().msec());
    return rng;
}

std::string Generators::generate_yokin_name(size_t length) {
    const static std::vector<Cluster> consonant_clusters = {
        {"N", 1.0},
        {"M", 1.4},
        {"G", 1.0},
        {"K", 1.0},
        {"L", 1.0},
        {"LL", 1.0},
        {"LH", 1.0},
        {"V", 1.0},
        {"R", 1.0},
        {"T", 1.0},
        {"D", 1.0},
        {"SH", 1.0},
        {"ZS", 1.0},
        {"Z", 1.0},
        {"S", 1.0},
        {"J", 0.5},
        {"DZ", 0.8},
        {"XX", 0.2},
    };

    const static std::vector<Cluster> vowel_clusters = {
        {"A", 0.8},
        {"E", 0.8},
        {"I", 0.3},
        {"AE", 1.5},
        {"EA", 1.0},
        {"AI", 1.0},
        {"U", 0.2},
        {"UH", 0.4},
        {"Y", 0.5},
        {"O", 0.2},
        {"II", 0.2},
    };

    bool consonant = rng()->generate() % 2;
    std::string name;

    while (name.length() < length) {
        name += consonant
            ? sample_with_weights(consonant_clusters)
            : sample_with_weights(vowel_clusters);
        consonant = !consonant;
    }

    if (name.length() > length) {
        name = name.substr(0, length);
    }

    return name;
}
