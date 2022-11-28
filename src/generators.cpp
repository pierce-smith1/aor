#include "generators.h"

QRandomGenerator *Generators::rng() {
    static QRandomGenerator *rng = new QRandomGenerator(QTime::currentTime().msec());
    return rng;
}

QString Generators::yokin_name(size_t length) {
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

    bool consonant {rng()->generate() % 2 == 0};
    QString name;

    while (name.length() < length) {
        name += consonant
            ? sample_with_weights(consonant_clusters)
            : sample_with_weights(vowel_clusters);
        consonant = !consonant;
    }

    return name.left(length);
}

std::vector<Item> Generators::base_items(const std::vector<Item> &inputs, const Item &tool, ItemDomain action) {
    std::vector<Item> outputs;
    const ItemProperties &tool_properties {tool.def()->properties};

    switch (action) {
        case Smithing: {
            if (inputs.size() != 3) {
                qFatal("Tried to smith without exactly 3 inputs (%zu)", inputs.size());
            }

            std::multiset<ItemCode> input_codes {
                inputs[0].code,
                inputs[1].code,
                inputs[2].code,
            };
            std::multiset<ItemCode> combo_codes {
                tool_properties[ToolComboIngredient1],
                tool_properties[ToolComboIngredient2],
                tool_properties[ToolComboIngredient3],
            };

            if (input_codes == combo_codes) {
                outputs.emplace_back(tool_properties[ToolComboResult]);
            } else {
                std::vector<std::pair<ItemCode, double>> clusters;
                for (const Item &item : inputs) {
                    clusters.emplace_back(item.def()->properties[MaterialForges], 1.0);
                }
                outputs.emplace_back(Generators::sample_with_weights(clusters));
            }

            break;
        }
        case Foraging:
        case Mining: {
            if (tool.id == EMPTY_ID) {
                if (action == Foraging) {
                    return {
                        Generators::sample_with_weights<Item>({
                            { Item("globfruit"), 1 },
                            { Item("byteberry"), 1 }
                        })
                    };
                } else {
                    return {
                        Generators::sample_with_weights<Item>({
                            { Item("obsilicon"), 1 },
                            { Item("oolite"), 1 }
                        })
                    };
                }
            }

            std::vector<std::pair<Item, double>> possible_discoveries;
            for (int i {(int) ToolCanDiscover1}; i <= (int) ToolCanDiscover9; i++) {
                if (tool_properties[(ItemProperty) i] != 0) {
                    possible_discoveries.emplace_back(
                        tool_properties[(ItemProperty) i],
                        tool_properties[(ItemProperty) (i + 9)]
                    );
                }
            }

            outputs.emplace_back(Generators::sample_with_weights(possible_discoveries));

            break;
        }
        case Praying: {
            if (tool.id == EMPTY_ID) {
                return {};
            }

            std::vector<std::pair<Item, double>> possible_discoveries;
            for (int i {(int) ToolCanDiscover1}; i <= (int) ToolCanDiscover9; i++) {
                if (tool_properties[(ItemProperty) i] != 0) {
                    possible_discoveries.emplace_back(
                        tool_properties[(ItemProperty) i],
                        tool_properties[(ItemProperty) (i + 9)]
                    );
                }
            }

            outputs.emplace_back(Generators::sample_with_weights(possible_discoveries));

            break;
        }
        default: {
            qWarning("Tried to generate items with unknown action domain (%d)", action);
        }
    }

    return outputs;
}

ItemId Generators::item_id() {
    auto time {std::chrono::system_clock::now().time_since_epoch()};
    auto milliseconds {std::chrono::duration_cast<std::chrono::milliseconds>(time).count()};

    return (milliseconds & 0xffffffff) + ((std::uint64_t) Generators::rng()->generate() << 32);
}

QColor Generators::color() {
    return sample_with_weights<QColor>({
        { QColor(255, 204, 0), 1 },
        { QColor(255, 153, 51), 1 },
        { QColor(255, 51, 0), 1, },
        { QColor(0, 102, 204), 1 },
        { QColor(0, 153, 255), 1 },
        { QColor(102, 102, 204), 1 },
        { QColor(0, 153, 0), 1 },
        { QColor(102, 204, 51), 1 },
    });
}
