#include "itemslot.h"
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
    Item item = get_item();

    if (item.id == EMPTY_ID) {
        return;
    }

    QEnterEvent *enter_event = (QEnterEvent *) event;
    game->item_tooltip.move(enter_event->globalPos());
    game->item_tooltip.set_text(item.get_tooltip_text());
    game->item_tooltip.widget.item_image->setPixmap(Item::pixmap_of(item));
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

    bool is_inventory_slot = get_type() == InventorySlot;
    bool item_being_used = get_item().intent != NoIntent;

    if (event->button() == Qt::RightButton && !is_inventory_slot && !game->activity_ongoing()) {
        drop_external_item();
        return;
    }

    if (event->button() == Qt::LeftButton && (!item_being_used || (!is_inventory_slot && !game->activity_ongoing()))) {
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;

        data->setText(objectName());
        drag->setMimeData(data);
        drag->setPixmap(Item::pixmap_of(item));

        drag->exec();
    }
}

void ItemSlot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data = event->mimeData();
    if (!data->hasFormat("text/plain")) {
        return;
    }

    event->acceptProposedAction();
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
