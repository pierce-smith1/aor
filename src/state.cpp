#include "state.h"
#include "itemslot.h"

ItemId State::get_item_id_at(int y, int x) const {
    return inventory[ItemSlot::inventory_index(y, x)].id;
}

Item &State::get_item_ref(ItemId id) {
    if (id == EMPTY_ID) {
        qFatal("Tried to get reference for the empty id");
    }

    auto match_id {[id](const Item &item) -> bool { return item.id == id; }};
    auto search_result {std::find_if(begin(inventory), end(inventory), match_id)};

    if (search_result == end(inventory)) {
        qFatal("Searching for an item by id turned up nothing (%lx)", id);
    }

    return *search_result;
}

Item State::get_item_instance(ItemId id) const {
    if (id == EMPTY_ID) {
        qWarning("Tried to get instance for the empty id");
        return Item();
    }

    auto match_id {[id](const Item &item) -> bool { return item.id == id; }};
    auto search_result {std::find_if(begin(inventory), end(inventory), match_id)};

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
    inventory[ItemSlot::inventory_index(y, x)] = item;
}

void State::remove_item_at(int y, int x) {
    inventory[ItemSlot::inventory_index(y, x)] = Item();
}

void State::remove_item_with_id(ItemId id) {
    for (size_t i {0}; i < INVENTORY_SIZE; i++) {
        if (inventory[i].id == id) {
            inventory[i] = Item();
            return;
        }
    }

    qWarning("Tried to remove item by id, but it didn't exist (%ld)", id);
}

bool State::add_item(const Item &item) {
    for (int y {0}; y < INVENTORY_ROWS; y++) {
        for (int x {0}; x < INVENTORY_COLS; x++) {
            if (inventory[ItemSlot::inventory_index(y, x)].id == EMPTY_ID) {
                inventory[ItemSlot::inventory_index(y, x)] = item;
                return true;
            }
        }
    }

    qWarning(
        "Tried to add item (code %d, id %ld) to inventory, but there was no open spot",
        item.code,
        item.id
    );
    return false;
}

ItemId State::make_item_at(ItemDefinitionPtr def, int y, int x) {
    Item new_item {Item(def)};
    inventory[ItemSlot::inventory_index(y, x)] = new_item;

    return new_item.id;
}

void State::mutate_item_at(std::function<void(Item &)> action, int y, int x) {
    action(inventory[ItemSlot::inventory_index(y, x)]);
}

void State::add_energy(std::uint16_t energy) {
    this->energy += energy;
    if (this->energy > BASE_MAX_ENERGY) {
        this->energy = BASE_MAX_ENERGY;
    }
}

void State::add_morale(std::uint16_t morale) {
    this->morale += morale;
    if (this->morale > BASE_MAX_MORALE) {
        this->morale = BASE_MAX_ENERGY;
    }
}

std::vector<Item> State::get_items_of_intent(ItemDomain intent) {
    std::vector<Item> items;

    for (size_t i {0}; i < INVENTORY_SIZE; i++) {
        if (inventory[i].intent == intent) {
            items.push_back(inventory[i]);
        }
    }

    return items;
}

State::State()
    : name(Generators::generate_yokin_name()), inventory(), activity({ None, 0 }) { }

void StateSerialize::save_state(const State &state, const QString &filename) {
    std::ofstream out {filename.toStdString()};

    put_char(out, 'l');
    put_char(out, 'k');
    put_short(out, STATE_VERSION);

    put_string(out, state.name);
    put_item_array(out, state.inventory);
    put_short(out, state.activity.action);
    put_long(out, state.activity.ms_left);
    put_long(out, state.activity.ms_total);
    put_id_array(out, state.external_item_ids.at(Material));
    put_id_array(out, state.external_item_ids.at(Offering));
    put_id_array(out, state.external_item_ids.at(Artifact));
    put_item_array(out, state.effects);
    put_long(out, state.tool_ids.at(SmithingTool));
    put_long(out, state.tool_ids.at(ForagingTool));
    put_long(out, state.tool_ids.at(MiningTool));
    put_long(out, state.tool_ids.at(PrayerTool));
    put_short(out, state.energy);
    put_short(out, state.morale);
}

State *StateSerialize::load_state(const QString &filename) {
    std::ifstream in(filename.toStdString());

    char header[4];
    header[0] = in.get();
    header[1] = in.get();
    header[2] = in.get();
    header[3] = in.get();

    if (header[0] != 'l' && header[1] != 'k') {
        qFatal(
            "Character file (%s) does not start with proper header (%c%c)",
            filename.toStdString().c_str(),
            header[0],
            header[1]
        );
    }

    State *state = new State;
    state->name = get_string(in);
    state->inventory = get_item_array<INVENTORY_SIZE>(in);
    state->activity.action = (ItemDomain) get_short(in);
    state->activity.ms_left = get_long(in);
    state->activity.ms_total = get_long(in);
    state->external_item_ids[Material] = get_id_array<MAX_ARRAY_SIZE>(in);
    state->external_item_ids[Offering] = get_id_array<MAX_ARRAY_SIZE>(in);
    state->external_item_ids[Artifact] = get_id_array<MAX_ARRAY_SIZE>(in);
    state->effects = get_item_array<EFFECT_SLOTS>(in);
    state->tool_ids[SmithingTool] = get_long(in);
    state->tool_ids[ForagingTool] = get_long(in);
    state->tool_ids[MiningTool] = get_long(in);
    state->tool_ids[PrayerTool] = get_long(in);
    state->energy = get_long(in);
    state->morale = get_long(in);

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

void StateSerialize::put_string(std::ostream &out, const QString &s) {
    put_short(out, s.size());
    for (char c : s.toUtf8()) {
        out.put(c);
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

QString StateSerialize::get_string(std::istream &in) {
    std::uint16_t size = get_short(in);

    char str[1 << 16];
    in.read(str, size);
    str[size] = '\0';

    return QString(str);
}
