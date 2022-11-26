#pragma once

#include <algorithm>

#include "state.h"

struct Character {
    Character();

    QString &name();

    CharacterActivity &activity();
    bool activity_ongoing();
    bool can_do(ItemDomain action);
    std::vector<Item> action_inputs();
    std::vector<Item> items_of_intent(ItemDomain domain);
    std::vector<Item> make_outputs();

    int energy();
    int morale();
    int max_energy();
    int max_morale();
    void add_energy(int add);
    void add_morale(int add);
    int energy_gain();
    int morale_gain();
    int base_morale_cost();

    Item item(ItemId id);
    Item item(int y, int x);
    Item &item_ref(ItemId id);
    Item &item_ref(int y, int x);
    void remove_item(ItemId id);
    void remove_item(int y, int x);
    void put_item(const Item &item, int y, int x);
    bool give_item(Item item);
    Item tool();
    Item tool(ItemDomain domain);
    ToolIds &tools();
    ExternalItemIds &external_items();
    Effects &effects();

    TooltipText tooltip_text(const Item &item);

    static int accumulate_property(const std::vector<Item> &items, ItemProperty prop);

    State state;
};
