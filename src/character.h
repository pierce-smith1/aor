#pragma once

#include <algorithm>

#include "state.h"

struct Character {
    Character();

    CharacterActivity &activity();
    bool activity_ongoing();
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
    Item tool(ItemDomain domain);
    ToolIds &tools();
    ExternalItemIds &external_items();
    Effects &effects();
    std::vector<Item> inputs();

    QString name();

    std::vector<Item> items_of_intent(ItemDomain domain);
    int accumulate_property(const std::vector<Item> &items, ItemProperty prop);

    bool can_do(ItemDomain action);

    State state;
};
