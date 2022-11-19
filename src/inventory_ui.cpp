#include "inventory_ui.h"
#include "items.h"
#include "qnamespace.h"

ItemSlot::ItemSlot(LKGameWindow *game)
    : QFrame(game->window.inventory_group), y(INVALID_COORD), x(INVALID_COORD), game(game)
{
    setMinimumSize(QSize(56, 56));
    setMaximumSize(QSize(56, 56));
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setMouseTracking(true);
    setAcceptDrops(true);

    QLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    item_layout = layout;

    QLabel *label = new QLabel(this);
    label->setMinimumSize(QSize(48, 48));
    label->setMaximumSize(QSize(48, 48));
    label->setMouseTracking(true);
    item_label = label;

    layout->addWidget(label);

    opacity_effect.setOpacity(1.0);
    item_label->setGraphicsEffect(&opacity_effect);
}

ItemSlot::ItemSlot(LKGameWindow *game, int y, int x)
    : ItemSlot(game)
{
    setObjectName(make_internal_name("inventory_slot", y, x));
    item_layout->setObjectName(make_internal_name("inventory_layout", y, x));
    item_label->setObjectName(make_internal_name("inventory_label", y, x));

    game->register_slot_name(objectName().toStdString());

    this->y = y;
    this->x = x;
}

Item ItemSlot::get_item() {
    return game->read_state<Item>([=](const State &state) {
        return state.inventory[inventory_index(y, x)];
    });
}

void ItemSlot::set_item(const Item &item) {
    game->mutate_state([=](State &state) {
        state.copy_item_to(item, y, x);
    });
}

SlotType ItemSlot::get_type() {
    return InventorySlot;
}

void ItemSlot::refresh_pixmap() {
    Item item = get_item();

    item_label->setPixmap(Item::pixmap_of(item));

    if (item.intent != NoIntent && get_type() == InventorySlot) {
        opacity_effect.setOpacity(0.5);
    } else {
        opacity_effect.setOpacity(1.0);
    }
}

std::vector<ItemSlot *> ItemSlot::get_slots_of_same_type() {
    std::vector<ItemSlot *> item_slots;

    for (const std::string &name : game->get_item_slot_names()) {
        ItemSlot *slot = findChild<ItemSlot *>(QString::fromStdString(name));
        if (slot->get_type() == get_type()) {
            item_slots.push_back(slot);
        }
    }

    return item_slots;
}

void ItemSlot::insert_inventory_slots(LKGameWindow &window) {
    for (unsigned x = 0; x < INVENTORY_COLS; x++) {
        for (unsigned y = 0; y < INVENTORY_ROWS; y++) {
            insert_inventory_slot(window, y, x);
        }
    }
}

void ItemSlot::insert_inventory_slot(LKGameWindow &window, unsigned y, unsigned x) {
    QGridLayout *inventory_grid = dynamic_cast<QGridLayout*>(window.window.inventory_group->layout());
    inventory_grid->addWidget(new ItemSlot(&window, y, x), y, x);
}

bool ItemSlot::are_yx_coords_oob(int y, int x) {
    return (y < 0 || y >= INVENTORY_ROWS) || (x < 0 || x >= INVENTORY_COLS);
}

size_t ItemSlot::inventory_index(int y, int x) {
    return y * INVENTORY_COLS + x;
}

QString ItemSlot::make_internal_name(const std::string &base, int y, int x) {
    return QString::fromStdString(base + ";" + std::to_string(y) + ":" + std::to_string(x));
}

void ItemSlot::enterEvent(QEvent *event) {
    if (get_item().id == EMPTY_ID) {
        return;
    }

    QEnterEvent *enter_event = (QEnterEvent *) event;
    game->item_tooltip.move(enter_event->globalPos());

    Ui::Tooltip &tooltip = game->item_tooltip.widget;
    tooltip.item_name->setText(QString::fromStdString(
        "<b>" + Item::def_of(get_item())->display_name + "</b>"
    ));
    tooltip.item_description->setText(QString::fromStdString(
        "<i>" + Item::def_of(get_item())->description + "</i>"
    ));
    tooltip.item_subtext->setText(QString::fromStdString(
        Item::type_to_string(Item::def_of(get_item())->type)
    ));
    tooltip.item_image->setPixmap(Item::pixmap_of(get_item()));

    switch (get_item().intent) {
        default:
        case NoIntent: {
            break;
        }
        case ToBeMaterial: {
            tooltip.item_subtext->setText(tooltip.item_subtext->text()
                + " <b><font color=green>(Queued for smithing)</font></b>");
            break;
        }
        case ToBeOffered: {
            tooltip.item_subtext->setText(tooltip.item_subtext->text()
                + " <b><font color=green>(Queued for offering)</font></b>");
            break;
        }
        case UsingAsTool: {
            tooltip.item_subtext->setText(tooltip.item_subtext->text()
                + " <b><font color=green>(Current tool)</font></b>");
            break;
        }
        case UsingAsArtifact: {
            tooltip.item_subtext->setText(tooltip.item_subtext->text()
                + " <b><font color=green>(Active artifact)</font></b>");
            break;
        }
    }

    game->item_tooltip.show();
}

void ItemSlot::mouseMoveEvent(QMouseEvent *event) {
    if (get_item().id != EMPTY_ID) {
        game->item_tooltip.move(event->globalPos());
    }
}

void ItemSlot::leaveEvent(QEvent *) {
    game->item_tooltip.hide();
}

void ItemSlot::mousePressEvent(QMouseEvent *event) {
    Item item = get_item();

    if (item.id == EMPTY_ID) {
        return;
    }

    if (event->button() == Qt::RightButton && get_type() != InventorySlot) {
        drop_external_item();
        return;
    }

    bool draggable = get_type() != InventorySlot || get_item().intent == NoIntent;
    if (event->button() == Qt::LeftButton && draggable) {
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;

        data->setText(objectName());
        drag->setMimeData(data);
        drag->setPixmap(Item::pixmap_of(item));

        drag->exec();
    }
}

void ItemSlot::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("text/plain")) {
        event->acceptProposedAction();
    }
}

void ItemSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = game->findChild<ItemSlot *>(source_slot_name);

    if (source_slot->get_type() == InventorySlot) {
        // Dragging between inventory slots swaps the items in each slot.
        game->mutate_state([=](State &state) {
            Item source_item = source_slot->get_item();
            Item dest_item = get_item();

            state.copy_item_to(source_item, y, x);
            state.copy_item_to(dest_item, source_slot->y, source_slot->x);
        });
    } else {
        // Dragging from an external slot to an inventory slot clears the external
        // slot and returns the intent of the original item to NoIntent.
        source_slot->drop_external_item();
        refresh_pixmap();
    }
}

void ItemSlot::drop_external_item() {
    game->mutate_state([=](State &state) {
        ItemId external_item_id = get_item().id;
        state.get_item_ref(external_item_id).intent = NoIntent;
    });
    set_item(Item());

    refresh_pixmap();
}

ExternalSlot::ExternalSlot(LKGameWindow *game, SlotType type, int n)
    : ItemSlot(game), type(type), n(n), held_item_id(EMPTY_ID)
{
    setObjectName(make_internal_name("external_slot;", type, n));
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

void ExternalSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = game->findChild<ItemSlot *>(source_slot_name);

    ItemIntent intent_to_assign;
    switch (get_type()) {
        case MaterialSlot: intent_to_assign = ToBeMaterial; break;
        case PrayerSlot:   intent_to_assign = ToBeOffered; break;
        case ToolSlot:     intent_to_assign = UsingAsTool; break;
        case ArtifactSlot: intent_to_assign = UsingAsArtifact; break;
        default:           intent_to_assign = NoIntent; break;
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
    } else {
        // Dragging between external slots of different types is not allowed.
    }
}

void ExternalSlot::insert_external_slots(LKGameWindow &window) {
    QGridLayout *smith_layout = window.findChild<QGridLayout *>("smith_layout");
    QGridLayout *prayer_layout = window.findChild<QGridLayout *>("prayer_layout");
    QVBoxLayout *aritfact_layout = window.findChild<QVBoxLayout *>("artifact_layout");

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
