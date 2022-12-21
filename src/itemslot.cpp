#include "itemslot.h"
#include "items.h"
#include "qnamespace.h"

ItemSlot::ItemSlot()
    : Hoverable<QFrame>(gw()->tooltip(), gw()),
      y(INVALID_COORD),
      x(INVALID_COORD),
      m_opacity_effect(new QGraphicsOpacityEffect(this))
{
    setMinimumSize(QSize(56, 56));
    setMaximumSize(QSize(56, 56));
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setMouseTracking(true);
    setAcceptDrops(true);

#ifdef _WIN32
    setProperty("slot", true);
#endif

    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_item_layout = layout;

    QLabel *label = new QLabel(this);
    label->setMinimumSize(QSize(48, 48));
    label->setMaximumSize(QSize(48, 48));
    label->setMouseTracking(true);
    m_item_label = label;

    layout->addWidget(label, 0, 0);

    m_opacity_effect->setOpacity(1.0);
    m_item_label->setGraphicsEffect(m_opacity_effect);
}

ItemSlot::ItemSlot(int y, int x)
    : ItemSlot()
{
    setObjectName(make_internal_name("inventory_slot", y, x));
    m_item_layout->setObjectName(make_internal_name("inventory_layout", y, x));
    m_item_label->setObjectName(make_internal_name("inventory_label", y, x));

    gw()->register_slot(this);

    this->y = y;
    this->x = x;
}

Item ItemSlot::get_item() {
    return gw()->game().inventory().get_item(y, x);
}

void ItemSlot::set_item(const Item &item) {
    gw()->game().inventory().put_item(item, y, x);
}

ItemDomain ItemSlot::type() {
    return Ordinary;
}

void ItemSlot::refresh_pixmap() {
    Item item = get_item();
    m_item_label->setPixmap(Item::pixmap_of(item));

    if (item.intent != None && type() == Ordinary) {
        m_opacity_effect->setOpacity(0.5);
    } else {
        m_opacity_effect->setOpacity(1.0);
    }
}

void ItemSlot::drop_external_item() {
    Item item_to_drop = get_item();
    if (item_to_drop.id == EMPTY_ID) {
        return;
    }

    ItemId external_item_id = get_item().id;
    gw()->game().inventory().get_item_ref(external_item_id).intent = None;
    set_item(Item());

    // Hacky: if we dropped a smithing tool, unslot all of the active materials,
    // because we may no longer be able to support them without the tool's power.
    if (item_to_drop.def()->type & SmithingTool) {
        for (ItemSlot *slot : gw()->item_slots(Material)) {
            if (slot->get_item().id != EMPTY_ID) {
                gw()->game().inventory().get_item_ref(slot->get_item().id).intent = None;
                slot->set_item(Item());
            }
        }
    }

    refresh_pixmap();
    gw()->refresh_ui();
}

void ItemSlot::insert_inventory_slots() {
    for (unsigned x = 0; x < INVENTORY_COLS; x++) {
        for (unsigned y = 0; y < INVENTORY_ROWS; y++) {
            insert_inventory_slot(y, x);
        }
    }
}

void ItemSlot::insert_inventory_slot(unsigned y, unsigned x) {
    QGridLayout *inventory_grid = dynamic_cast<QGridLayout*>(gw()->window().inventory_slots->layout());
    inventory_grid->addWidget(new ItemSlot(y, x), y, x);
}

QString ItemSlot::make_internal_name(const QString &base, int y, int x) {
    return QString("%1;%2:%3").arg(base).arg(y).arg(x);
}

bool ItemSlot::do_hovering() {
    return get_item().id != EMPTY_ID;
}

std::optional<Item> ItemSlot::tooltip_item() {
    return std::optional<Item>(get_item());
}

void ItemSlot::mousePressEvent(QMouseEvent *event) {
    Item item = get_item();

    if (item.id == EMPTY_ID) {
        return;
    }

    bool is_inventory_slot = type() == Ordinary;
    bool item_being_used = get_item().intent != None;
    bool is_current_activity_unrelated = type() != gw()->selected_char().activity().action();

    if (event->button() == Qt::RightButton
        && !is_inventory_slot
        && is_current_activity_unrelated
    ) {
        drop_external_item();
        return;
    }

    if (event->button() == Qt::LeftButton
        && (!item_being_used || (!is_inventory_slot && !gw()->selected_char().activity().ongoing()))
        && !gw()->selected_char().dead()
    ) {
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

    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = gw()->findChild<ItemSlot *>(source_slot_name);

    if (source_slot->type() == Explorer) {
        return;
    }

    event->acceptProposedAction();
}

void ItemSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = gw()->findChild<ItemSlot *>(source_slot_name);

    if (source_slot->type() == Ordinary) {
        // Dragging between inventory slots swaps the items in each slot.
        Item source_item = source_slot->get_item();
        Item dest_item = get_item();

        gw()->game().inventory().put_item(source_item, y, x);
        gw()->game().inventory().put_item(dest_item, source_slot->y, source_slot->x);

        source_slot->refresh_pixmap();
    } else {
        // Dragging from an external slot to an inventory slot clears the external
        // slot and returns the intent of the original item to NoIntent.
        source_slot->drop_external_item();
    }

    gw()->refresh_ui();
}
