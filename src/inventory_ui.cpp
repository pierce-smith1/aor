#include "inventory_ui.h"

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

void ItemSlot::refresh_pixmap() {
    item_label->setPixmap(Item::pixmap_of(get_item()));
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

void ItemSlot::enterEvent(QEvent *event) {
    QEnterEvent *enter_event = (QEnterEvent *) event;
    game->item_tooltip.move(enter_event->globalPos());

    game->item_tooltip.widget.item_name->setText(QString::fromStdString(
        "**" + Item::def_of(get_item())->display_name + "**"
    ));
    game->item_tooltip.widget.item_description->setText(QString::fromStdString(
        "*" + Item::def_of(get_item())->description + "*"
    ));
    game->item_tooltip.widget.item_subtext->setText(QString::fromStdString(
        Item::type_to_string(Item::def_of(get_item())->type)
    ));
    game->item_tooltip.widget.item_image->setPixmap(
        Item::pixmap_of(get_item())
    );
    game->item_tooltip.show();
}

void ItemSlot::mouseMoveEvent(QMouseEvent *event) {
    game->item_tooltip.move(event->globalPos());
}

void ItemSlot::leaveEvent(QEvent *) {
    game->item_tooltip.hide();
}

void ItemSlot::mousePressEvent(QMouseEvent *event) {
    Item item = get_item();

    if (item.id == EMPTY_ID) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
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

    game->mutate_state([=](State &state) {
        Item source_item = state.get_item_instance_at(source_slot->y, source_slot->x);
        Item dest_item = state.get_item_instance_at(this->y, this->x);

        state.copy_item_to(source_item, this->y, this->x);
        state.copy_item_to(dest_item, source_slot->y, source_slot->x);
    });
}

QString ItemSlot::make_internal_name(const std::string &base, int y, int x) {
    return QString::fromStdString(base + ";" + std::to_string(y) + ":" + std::to_string(x));
}

ExternalSlot::ExternalSlot(LKGameWindow *game, ExternalSlotType type, int n)
    : ItemSlot(game), type(type), held_item_id(EMPTY_ID)
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

void ExternalSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = game->findChild<ItemSlot *>(source_slot_name);

    game->mutate_state([=](State &state) {
        Item &item = state.inventory[inventory_index(source_slot->y, source_slot->x)];
        item.intent = ToBeMaterial;
        held_item_id = item.id;
    });
}

void ExternalSlot::insert_external_slots(LKGameWindow &window) {
    QGridLayout *smith_layout = window.findChild<QGridLayout *>("smith_layout");
    QGridLayout *prayer_layout = window.findChild<QGridLayout *>("prayer_layout");

    for (int i = 0; i < 6; i++) {
        smith_layout->addWidget(new ExternalSlot(&window, MaterialSlot, i), i / 3, i % 3);
        prayer_layout->addWidget(new ExternalSlot(&window, PrayerSlot, i), i / 3, i % 3);
    }
}
