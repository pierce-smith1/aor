#include "actions.h"

CharacterActivity::CharacterActivity(ItemDomain action, std::int64_t ms)
    : action(action), ms_left(ms), ms_total(ms) { }

std::vector<Item> Actions::generate_items(const std::vector<Item> &inputs, const Item &tool, ItemDomain action) {
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
        default: {
            qWarning("Tried to generate items with unknown action domain (%d)", action);
        }
    }

    return outputs;
}
