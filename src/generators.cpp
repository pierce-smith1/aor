#include "generators.h"
#include "character.h"

QRandomGenerator *Generators::rng() {
    static QRandomGenerator *rng = new QRandomGenerator(QTime::currentTime().msec());
    return rng;
}

QString Generators::yokin_name() {
    qint32 length = 3;
    while (percent_chance(75 - ((length - 3) * 15))) {
        length++;
    }

    const static std::vector<Cluster> consonant_clusters = {
        {"TH", 1.5},
        {"R", 1.0},
        {"RH", 1.0},
        {"N", 1.0},
        {"L", 0.8},
        {"K", 0.9},
        {"KH", 0.9},
        {"D", 1.2},
        {"SH", 1.0},
        {"C", 1.2},
        {"ST", 1.2},
        {"CH", 0.9},
        {"SS", 0.5},
        {"NT", 0.5},
        {"ND", 0.9},
        {"M", 1.0}
    };

    const static std::vector<Cluster> vowel_clusters = {
        {"A", 1.5},
        {"E", 1.5},
        {"I", 1.0},
        {"Y", 0.2},
        {"O", 1.1},
        {"U", 0.2},
    };

    bool consonant = rng()->generate() % 2 == 0;
    QString name;

    while (name.length() < length) {
        name += consonant
            ? sample_with_weights(consonant_clusters)
            : sample_with_weights(vowel_clusters);
        consonant = !consonant;
    }

    return name.front() + name.mid(1, length - 1).toCaseFolded();
}

QString Generators::tribe_name() {
    const static std::vector<Cluster> LOCATION_ADJECTIVES = {
        {"Radiant", 1.0},
        {"Lightning", 1.0},
        {"Volum", 1.0},
        {"Wire", 1.0},
        {"Soldered", 1.0},
        {"Locked", 1.0},
        {"Thread", 1.0},
        {"Striped", 1.0},
        {"Sync", 1.0},
        {"Short", 1.0},
        {"Frozen", 1.0},
        {"Forged", 1.0},
        {"Star", 1.0},
        {"Hash", 1.0},
        {"Melted", 1.0},
        {"Serde", 1.0},
        {"Rusted", 1.0},
        {"Pearl", 1.0},
    };

    const static std::vector<Cluster> LOCATION_NOUNS = {
        {"stone", 1.0},
        {"garden", 1.0},
        {"spines", 1.0},
        {"spires", 1.0},
        {"feathers", 1.0},
        {"mesa", 1.0},
        {"woods", 1.0},
        {"plains", 1.0},
        {"fields", 1.0},
        {"cliffs", 1.0},
        {"thorns", 1.0},
        {"canyons", 1.0},
        {"chasms", 1.0},
        {"hills", 1.0},
        {"crater", 1.0},
        {"caves", 1.0},
        {"caverns", 1.0},
        {"lake", 1.0}
    };

    return sample_with_weights(LOCATION_ADJECTIVES) + sample_with_weights(LOCATION_NOUNS);
}

ItemId Generators::item_id() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

    return ((milliseconds & 0xffffffff) + ((quint64) rng()->generate() << 32)) & 0x7fffffffffffffff;
}

Color Generators::color() {
    return sample_with_weights<Color>({
        { Banana, 1 },
        { Orange, 1 },
        { Cherry, 1, },
        { Blueberry, 1 },
        { Plum, 1 },
        { Cucumber, 1 },
        { Lime, 1 },
    });
}

GameId Generators::game_id() {
    return item_id().n;
}

CharacterId Generators::char_id() {
    return rng()->generate() % 0xffff;
}

ActivityId Generators::activity_id() {
    return item_id().n;
}

bool Generators::percent_chance(int p) {
    return ((int) (rng()->generate() % 100)) < p;
}
