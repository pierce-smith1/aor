#include "state.h"
#include "generators.h"
#include "inventory_ui.h"

ItemId State::get_item_id_at(int y, int x) const {
    return inventory[ItemSlot::inventory_index(y, x)].id;
}

Item &State::get_item_ref(ItemId id) {
    auto match_id = [id](const Item &item) -> bool { return item.id == id; };
    auto search_result = std::find_if(begin(inventory), end(inventory), match_id);

    if (search_result == end(inventory)) {
        qFatal("Searching for an item by id turned up nothing (%lx)", id);
    }

    return *search_result;
}

Item State::get_item_instance(ItemId id) const {
    auto match_id = [id](const Item &item) -> bool { return item.id == id; };
    auto search_result = std::find_if(begin(inventory), end(inventory), match_id);

    if (search_result == end(inventory)) {
        qFatal("Searching for an item by id turned up nothing (%lx)", id);
    }

    return *search_result;
}

Item &State::get_item_ref_at(int y, int x) {
    return inventory[ItemSlot::inventory_index(y, x)];
}

Item State::get_item_instance_at(int y, int x) const {
    return inventory[ItemSlot::inventory_index(y, x)];
}

void State::copy_item_to(const Item &item, int y, int x) {
    if (inventory[ItemSlot::inventory_index(y, x)].id != EMPTY_ID) {
        qWarning("Placed an item (id: %lx, code: %d) into a non-empty inventory space (y: %d, x: %d)", item.id, item.code, y, x);
    }

    inventory[ItemSlot::inventory_index(y, x)] = item;
}

void State::remove_item_at(int y, int x) {
    inventory[ItemSlot::inventory_index(y, x)] = Item();
}

ItemId State::make_item_at(ItemDefinitionPtr def, int y, int x) {
    if (inventory[ItemSlot::inventory_index(y, x)].id != EMPTY_ID) {
        qWarning("Made an item (code: %d) at a non-empty inventory space (y: %d, x: %d)", def->code, y, x);
    }

    Item new_item = Item(def);
    inventory[ItemSlot::inventory_index(y, x)] = new_item;

    return new_item.id;
}

void State::mutate_item_at(std::function<void(Item &)> action, int y, int x) {
    action(inventory[ItemSlot::inventory_index(y, x)]);
}

State::State()
    : name(Generators::generate_yokin_name()), inventory(), activity({ Nothing, 0 }) { }

void StateSerialize::save_state(const State &state, const std::string &filename) {
    std::ofstream out(filename);

    put_char(out, 'l');
    put_char(out, 'k');
    put_short(out, STATE_VERSION);

    put_string(out, state.name);
    put_inventory(out, state.inventory);
    put_char(out, state.activity.action);
    put_long(out, state.activity.ms_left);
    put_id_array(out, state.materials);
    put_id_array(out, state.offered_items);
    put_id_array(out, state.artifacts);
    put_long(out, state.tool);
    put_short(out, state.energy);
}

State *StateSerialize::load_state(const std::string &filename) {
    std::ifstream in(filename);

    char header[4];
    header[0] = in.get();
    header[1] = in.get();
    header[2] = in.get();
    header[3] = in.get();

    if (header[0] != 'l' && header[1] != 'k') {
        qWarning("Character file (%s) does not start with proper header (%c%c)", filename.c_str(), header[0], header[1]);
        return nullptr;
    }

    State *state = new State;
    state->name = get_string(in);
    state->inventory = get_inventory(in);
    state->activity.action = (CharacterAction) get_char(in);
    state->activity.ms_left = get_long(in);
    state->materials = get_id_array<SMITHING_SLOTS>(in);
    state->offered_items = get_id_array<PRAYER_SLOTS>(in);
    state->artifacts = get_id_array<ARTIFACT_SLOTS>(in);
    state->tool = get_long(in);
    state->energy = get_short(in);

    return state;
}

void StateSerialize::put_char(std::ostream &out, unsigned char c) {
    out.put(c);
}

void StateSerialize::put_short(std::ostream &out, std::uint16_t n) {
    out.put(n & 0xff);
    out.put((n & 0xff00) >> 8);
}

void StateSerialize::put_long(std::ostream &out, std::uint64_t n) {
    out.put(n & 0xff);
    out.put((n & 0xff00) >> 8);
    out.put((n & 0xff0000) >> 16);
    out.put((n & 0xff000000) >> 24);
    out.put((n & 0xff00000000) >> 32);
    out.put((n & 0xff0000000000) >> 40);
    out.put((n & 0xff000000000000) >> 48);
    out.put((n & 0xff00000000000000) >> 56);
}

void StateSerialize::put_string(std::ostream &out, const std::string &s) {
    put_short(out, s.size());
    for (char c : s) {
        out.put(c);
    }
}

void StateSerialize::put_inventory(std::ostream &out, const Inventory &i)  {
    put_short(out, i.size());
    for (const Item &item : i) {
        put_short(out, item.code);
        put_long(out, item.id);
        put_char(out, item.uses_left);
        put_char(out, item.intent);
    }
}

unsigned char StateSerialize::get_char(std::istream &in) {
    return in.get();
}

std::uint16_t StateSerialize::get_short(std::istream &in) {
    std::uint16_t n = 0;

    n += in.get();
    n += in.get() << 8;

    return n;
}

std::uint64_t StateSerialize::get_long(std::istream &in) {
    std::uint64_t n = 0;

    n += (std::uint64_t) in.get();
    n += (std::uint64_t) in.get() << 8;
    n += (std::uint64_t) in.get() << 16;
    n += (std::uint64_t) in.get() << 24;
    n += (std::uint64_t) in.get() << 32;
    n += (std::uint64_t) in.get() << 40;
    n += (std::uint64_t) in.get() << 48;
    n += (std::uint64_t) in.get() << 56;

    return n;
}

std::string StateSerialize::get_string(std::istream &in) {
    std::uint16_t size = get_short(in);

    char str[1 << 16];
    in.read(str, size);
    str[size] = '\0';

    return std::string(str);
}

Inventory StateSerialize::get_inventory(std::istream &in) {
    std::uint16_t size = get_short(in);
    Inventory inventory;

    for (std::uint16_t i = 0; i < size; i++) {
        Item item;
        item.code = get_short(in);
        item.id = get_long(in);
        item.uses_left = get_char(in);
        item.intent = (ItemIntent) get_char(in);

        inventory[i] = item;
    }

    return inventory;
}
