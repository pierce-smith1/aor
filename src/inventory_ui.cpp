#include "inventory_ui.h"

using namespace InventoryUi;

void InventoryUi::insert_inventory_slots(LKGameWindow &window) {
    for (unsigned x = 0; x < INVENTORY_COLS; x++) {
        for (unsigned y = 0; y < INVENTORY_ROWS; y++) {
            insert_inventory_slot(window, y, x);
        }
    }
}

void InventoryUi::insert_inventory_slot(LKGameWindow &window, unsigned y, unsigned x) {
    QGridLayout *inventory_grid = dynamic_cast<QGridLayout*>(window.window.inventory_group->layout());
    inventory_grid->addWidget(make_inventory_slot(window, y, x), y, x);
}

QFrame *InventoryUi::make_inventory_slot(LKGameWindow &window, unsigned y, unsigned x) {
    QFrame *slot = new QFrame(window.window.inventory_group);

    slot->setObjectName(make_internal_name("inventory_slot", y, x));
    slot->setMinimumSize(QSize(56, 56));
    slot->setMaximumSize(QSize(56, 56));
    slot->setFrameShape(QFrame::StyledPanel);
    slot->setFrameShadow(QFrame::Raised);

    QLayout *layout = new QHBoxLayout(slot);
    layout->setSpacing(0);
    layout->setObjectName(make_internal_name("inventory_layout", y, x));
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *label = new QLabel(slot);
    label->setObjectName(make_internal_name("inventory_label", y, x));
    label->setMinimumSize(QSize(48, 48));
    label->setMaximumSize(QSize(48, 48));

    static InventoryEventFilter *inventory_event_filter = new InventoryEventFilter(&window);
    label->installEventFilter(inventory_event_filter);

    layout->addWidget(label);

    return slot;
}

QLabel *InventoryUi::get_inventory_label(LKGameWindow &window, unsigned y, unsigned x) {
    QString name = make_internal_name("inventory_label", y, x);
    QLabel *label = window.window.centralwidget->findChild<QLabel*>(name);

    if (label == nullptr) {
        qFatal("Failed to find an inventory label (%s)", name.toStdString().c_str());
    }

    return label;
}

QString InventoryUi::make_internal_name(const std::string &base, unsigned y, unsigned x) {
    return QString::fromStdString(base + ";" + std::to_string(y) + ":" + std::to_string(x));
}

std::pair<int, int> InventoryUi::get_yx_coords_of_label(QObject *label) {
    QString name = label->objectName();

    QStringList coords = name.split(";")[1].split(":");

    bool conversion_ok;
    int y = coords[0].toInt(&conversion_ok);
    if (!conversion_ok) {
        qFatal("Tried to get coords from malformed label name (%s)", name.toStdString().c_str());
    }

    int x = coords[1].toInt(&conversion_ok);
    if (!conversion_ok) {
        qFatal("Tried to get coords from malformed label name (%s)", name.toStdString().c_str());
    }

    return std::make_pair(y, x);
}

ItemId InventoryUi::get_item_in(LKGameWindow &window, QObject *label) {
    std::pair<int, int> coords = get_yx_coords_of_label(label);
    return window.character.inventory[coords.first * INVENTORY_COLS + coords.second].id;
}

bool InventoryUi::are_yx_coords_oob(int y, int x) {
    return (y < 0 || y >= INVENTORY_ROWS) || (x < 0 || x >= INVENTORY_COLS);
}

size_t InventoryUi::inventory_index(int y, int x) {
    return y * INVENTORY_COLS + x;
}
