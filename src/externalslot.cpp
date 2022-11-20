#include "externalslot.h"
#include "items.h"
#include "itemslot.h"

ExternalSlot::ExternalSlot(LKGameWindow *game, SlotType type, int n)
    : ItemSlot(game), type(type), n(n), held_item_id(EMPTY_ID)
{
    setObjectName(make_internal_name("external_slot", type, n));
    item_layout->setObjectName(make_internal_name("external_layout", type, n));
    item_label->setObjectName(make_internal_name("external_label", type, n));

    game->register_slot_name(objectName().toStdString());
}

Item ExternalSlot::get_item() {
    return game->read_state<Item>([=](const State &state) {
        return state.get_item_instance(held_item_id);
    });
}

void ExternalSlot::set_item(const Item &item) {
    held_item_id = item.id;

    switch (get_type()) {
        case MaterialSlot: {
            game->mutate_state([=](State &state) { state.materials[n] = get_item().id; });
            break;
        }
        case PrayerSlot: {
            game->mutate_state([=](State &state) { state.offered_items[n] = get_item().id; });
            break;
        }
        case ArtifactSlot: {
            game->mutate_state([=](State &state) { state.artifacts[n] = get_item().id; });
            break;
        }
        case ToolSlot: {
            game->mutate_state([=](State &state) { state.tool = get_item().id; });
            break;
        }
        default: {
            break;
        }
    }
}

SlotType ExternalSlot::get_type() {
    return type;
}

void ExternalSlot::refresh_pixmap() {
    // After a reload, we need to refresh the item behind this external slot
    ItemId id;
    switch (get_type()) {
        default:
        case InventorySlot: {
            ItemSlot::refresh_pixmap();
            return;
        }
        case MaterialSlot: {
            id = game->read_state<ItemId>([=](const State &state) {
                return state.materials[n];
            });
            break;
        }
        case PrayerSlot: {
            id = game->read_state<ItemId>([=](const State &state) {
                return state.offered_items[n];
            });
            break;
        }
        case ArtifactSlot: {
            id = game->read_state<ItemId>([=](const State &state) {
                return state.artifacts[n];
            });
            break;
        }
        case ToolSlot: {
            id = game->read_state<ItemId>([=](const State &state) {
                return state.tool;
            });
            break;
        }
    }
    held_item_id = id;

    ItemSlot::refresh_pixmap();
}

void ExternalSlot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data = event->mimeData();
    if (!data->hasFormat("text/plain")) {
        return;
    }

    if (game->activity_ongoing()) {
        return;
    }

    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = game->findChild<ItemSlot *>(source_slot_name);
    Item dropped_item = source_slot->get_item();
    ItemType type = dropped_item.def()->type;

    switch (get_type()) {
        case MaterialSlot: {
            if (type & IT_MATERIAL) event->acceptProposedAction(); break;
        }
        case PrayerSlot: {
            if (!(type & IT_RUNE)) event->acceptProposedAction(); break;
        }
        case ToolSlot: {
            if (type & IT_TOOL) event->acceptProposedAction(); break;
        }
        case ArtifactSlot: {
            if (type & IT_ARTIFACT) event->acceptProposedAction(); break;
        }
        case PortraitSlot: {
            if (type & IT_CONSUMABLE) event->acceptProposedAction(); break;
        }
        default: {
            break;
        }
    }
}

void ExternalSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = game->findChild<ItemSlot *>(source_slot_name);

    ItemIntent intent_to_assign;
    switch (get_type()) {
        case MaterialSlot: { intent_to_assign = ToBeMaterial; break; }
        case PrayerSlot:   { intent_to_assign = ToBeOffered; break; }
        case ToolSlot:     { intent_to_assign = UsingAsTool; break; }
        case ArtifactSlot: { intent_to_assign = UsingAsArtifact; break; }
        case PortraitSlot: { intent_to_assign = ToBeEaten; break; }
        default:           { intent_to_assign = NoIntent; break; }
    }

    if (source_slot->get_type() == InventorySlot) {
        // Dragging from an inventory slot to an external slot requires the
        // intent of the dragged item to change.
        game->mutate_state([=](State &state) {
            Item &item = state.inventory[inventory_index(source_slot->y, source_slot->x)];
            item.intent = intent_to_assign;
            set_item(item);
        });
    } else if (source_slot->get_type() == get_type() && source_slot != this) {
        // Dragging between external slots of the same type is purely visual,
        // just change the slot that holds the item id.
        set_item(source_slot->get_item());
        source_slot->set_item(Item());

        source_slot->refresh_pixmap();
        refresh_pixmap();
    }
}

void ExternalSlot::insert_external_slots(LKGameWindow &window) {
    QGridLayout *smith_layout = window.window.smith_layout;
    QGridLayout *prayer_layout = window.window.prayer_layout;
    QLayout *aritfact_layout = window.window.artifact_box->layout();

    for (int i = 0; i < SMITHING_SLOTS; i++) {
        smith_layout->addWidget(
            new ExternalSlot(&window, MaterialSlot, i),
            i / SMITHING_SLOTS_PER_ROW,
            i % SMITHING_SLOTS_PER_ROW
        );
    }

    for (int i = 0; i < PRAYER_SLOTS; i++) {
        prayer_layout->addWidget(
            new ExternalSlot(&window, PrayerSlot, i),
            i / PRAYER_SLOTS_PER_ROW,
            i % PRAYER_SLOTS_PER_ROW
        );
    }

    for (int i = 0; i < 4; i++) {
        aritfact_layout->addWidget(new ExternalSlot(&window, ArtifactSlot, i));
    }
}

ToolSlot::ToolSlot(LKGameWindow *game)
    : ExternalSlot(game, SlotType::ToolSlot, 0)
{
    setMinimumSize(QSize(0, 80));
    setMaximumSize(QSize(10000, 80));
}

void ToolSlot::insert_tool_slot(LKGameWindow &window) {
    window.window.player_layout->addWidget(new ToolSlot(&window));
}

PortraitSlot::PortraitSlot(LKGameWindow *game)
    : ExternalSlot(game, SlotType::PortraitSlot, 0)
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

void PortraitSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = game->findChild<ItemSlot *>(source_slot_name);

    Item item = source_slot->get_item();

    if (source_slot->get_type() != InventorySlot) {
        return;
    }

    if (item.def()->type & IT_CONSUMABLE) {
        game->mutate_state([=](State &state) {
            state.get_item_ref(item.id).intent = ToBeEaten;
        });
        game->start_activity(CharacterActivity(Eating, 60 * 1000));
    }
}
