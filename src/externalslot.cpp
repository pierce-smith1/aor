#include "externalslot.h"
#include "items.h"
#include "itemslot.h"

ExternalSlot::ExternalSlot(LKGameWindow *game, ItemDomain type, int n)
    : ItemSlot(game), item_slot_type(type), n(n), held_item_id(EMPTY_ID)
{
    setObjectName(make_internal_name("external_slot", type, n));
    item_layout->setObjectName(make_internal_name("external_layout", type, n));
    item_label->setObjectName(make_internal_name("external_label", type, n));

    game->register_slot_name(objectName());
}

Item ExternalSlot::get_item() {
    if (held_item_id == EMPTY_ID) {
        return Item();
    }

    return game->read_state<Item>([=](const State &state) {
        return state.get_item_instance(held_item_id);
    });
}

void ExternalSlot::set_item(const Item &item) {
    held_item_id = item.id;

    game->mutate_state([=](State &state) {
        state.external_item_ids[get_item_slot_type()][n] = get_item().id;
    });
}

ItemDomain ExternalSlot::get_item_slot_type() {
    return item_slot_type;
}

void ExternalSlot::refresh_pixmap() {
    // After a reload, we need to refresh the item behind this external slot
    ItemId id;
    switch (get_item_slot_type()) {
        default:
        case Ordinary: {
            ItemSlot::refresh_pixmap();
            return;
        }
        case Material:
        case Offering:
        case Artifact: {
            id = game->read_state<ItemId>([=](const State &state) {
                return state.external_item_ids.at(get_item_slot_type())[n];
            });
            break;
        }
    }
    held_item_id = id;

    ItemSlot::refresh_pixmap();
}

void ExternalSlot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data {event->mimeData()};
    if (!data->hasFormat("text/plain")) {
        return;
    }

    if (game->activity_ongoing()) {
        return;
    }

    QString source_slot_name {event->mimeData()->text()};
    ItemSlot *source_slot {game->findChild<ItemSlot *>(source_slot_name)};
    Item dropped_item {source_slot->get_item()};
    ItemType type {dropped_item.def()->type};

    switch (get_item_slot_type()) {
        case Offering: {
            if (!(type & Rune)) { event->acceptProposedAction(); } break;
        }
        case Character: {
            if (type & Consumable) { event->acceptProposedAction(); } break;
        }
        default: {
            if (type & get_item_slot_type()) { event->acceptProposedAction(); } break;
        }
    }
}

void ExternalSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name {event->mimeData()->text()};
    ItemSlot *source_slot {game->findChild<ItemSlot *>(source_slot_name)};

    ItemDomain intent_to_assign {get_item_slot_type()};

    if (source_slot->get_item_slot_type() == Ordinary) {
        // Dragging from an inventory slot to an external slot requires the
        // intent of the dragged item to change.
        game->mutate_state([=](State &state) {
            Item &item = state.inventory[inventory_index(source_slot->y, source_slot->x)];
            item.intent = intent_to_assign;
            set_item(item);
        });
    } else if (source_slot->get_item_slot_type() == get_item_slot_type() && source_slot != this) {
        // Dragging between external slots of the same type is purely visual,
        // just change the slot that holds the item id.
        set_item(source_slot->get_item());
        source_slot->set_item(Item());

        source_slot->refresh_pixmap();
        refresh_pixmap();
    }
}

void ExternalSlot::insert_external_slots(LKGameWindow &window) {
    QGridLayout *smith_layout {window.window.smith_layout};
    QGridLayout *prayer_layout {window.window.prayer_layout};
    QLayout *aritfact_layout {window.window.artifact_box->layout()};

    for (int i {0}; i < SMITHING_SLOTS; i++) {
        smith_layout->addWidget(
            new ExternalSlot(&window, Material, i),
            i / SMITHING_SLOTS_PER_ROW + 1,
            i % SMITHING_SLOTS_PER_ROW
        );
    }

    for (int i {0}; i < PRAYER_SLOTS; i++) {
        prayer_layout->addWidget(
            new ExternalSlot(&window, Offering, i),
            i / PRAYER_SLOTS_PER_ROW + 1,
            i % PRAYER_SLOTS_PER_ROW
        );
    }

    for (int i {0}; i < ARTIFACT_SLOTS; i++) {
        aritfact_layout->addWidget(new ExternalSlot(&window, Artifact, i));
    }
}

ToolSlot::ToolSlot(LKGameWindow *game, ItemDomain type)
    : ExternalSlot(game, type, 0), tool_slot_type(type)
{
    setMinimumSize(QSize(0, 80));
    setMaximumSize(QSize(10000, 80));
}

void ToolSlot::set_item(const Item &item) {
    game->mutate_state([=](State &state) {
        state.tool_ids[get_tool_slot_type()] = item.id;
    });
}

void ToolSlot::refresh_pixmap() {
    held_item_id = game->read_state<ItemId>([=](const State &state) {
        return state.tool_ids.at(get_tool_slot_type());
    });
    ExternalSlot::refresh_pixmap();
}

ItemDomain ToolSlot::get_tool_slot_type() {
    return tool_slot_type;
}

void ToolSlot::insert_tool_slots(LKGameWindow &window) {
    window.window.smith_layout->addWidget(new ToolSlot(&window, SmithingTool), 0, 0, 1, 3);
    window.window.exploring_layout->addWidget(new ToolSlot(&window, ForagingTool), 0, 0);
    window.window.exploring_layout->addWidget(new ToolSlot(&window, MiningTool), 2, 0);
    window.window.prayer_layout->addWidget(new ToolSlot(&window, PrayerTool), 0, 0, 1, 3);
}

PortraitSlot::PortraitSlot(LKGameWindow *game)
    : ExternalSlot(game, Character, 0)
{
    setMinimumSize(QSize(0, 0));
    setMaximumSize(QSize(10000, 10000));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

Item PortraitSlot::get_item() {
    qFatal("Tried to get item from a portrait slot");
}

void PortraitSlot::set_item(const Item &item) {
    qFatal("Tried to set item of a portrait slot (code %d, id %ld)", item.code, item.id);
}

void PortraitSlot::refresh_pixmap() {
}

void PortraitSlot::insert_portrait_slot(LKGameWindow &window) {
    window.window.player_layout->addWidget(new PortraitSlot(&window));
}

void PortraitSlot::enterEvent(QEvent *) {
    return;
}

void PortraitSlot::mouseMoveEvent(QMouseEvent *) {
    return;
}

void PortraitSlot::mousePressEvent(QMouseEvent *) {
    return;
}

void PortraitSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name {event->mimeData()->text()};
    ItemSlot *source_slot {game->findChild<ItemSlot *>(source_slot_name)};
    Item item {source_slot->get_item()};

    if (source_slot->get_item_slot_type() != Ordinary) {
        return;
    }

    if (item.def()->type & Consumable) {
        game->mutate_state([=](State &state) {
            state.get_item_ref(item.id).intent = Consumable;
        });
        game->start_activity(CharacterActivity(Eating, 60 * 1000));
    }
}
