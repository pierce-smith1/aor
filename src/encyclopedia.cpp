#include "encyclopedia.h"
#include "die.h"

Encyclopedia::Encyclopedia()
    : m_widget()
{
    m_widget.setupUi(this);

    for (ItemCode group = CT_CONSUMABLE; group <= CT_ARTIFACT; group <<= 1) {
        QGroupBox *group_box = new QGroupBox(this);
        QGridLayout *box_layout = new QGridLayout;
        group_box->setLayout(box_layout);

        switch (group) {
            case CT_CONSUMABLE: { group_box->setTitle("Consumables"); break; }
            case CT_MATERIAL:   { group_box->setTitle("Materials"); break; }
            case CT_TOOL:       { group_box->setTitle("Tools"); break; }
            case CT_ARTIFACT:   { group_box->setTitle("Artifacts"); break; }
        }
        m_widget.encyclopedia_slots->layout()->addWidget(group_box);

        for (int y = 0; y < ENCYCLOPEDIA_GROUP_ROWS; y++) {
            for (int x = 0; x < ENCYCLOPEDIA_GROUP_COLS; x++) {
                EncyclopediaSlot *slot = new EncyclopediaSlot(y, x, group);
                box_layout->addWidget(slot, y, x);

                if (slot->get_item().id != EMPTY_ID) {
                    m_total_items++;
                }
            }
        }
    }
}

void Encyclopedia::refresh() {
    m_widget.progress_bar->setMaximum(m_total_items);

    int discovered = 0;
    for (EncyclopediaSlot *slot : findChildren<EncyclopediaSlot *>()) {
        if (!slot->undiscovered()) {
            discovered++;
        }
    }

    m_widget.progress_bar->setValue(discovered);
}

EncyclopediaSlot::EncyclopediaSlot(int y, int x, ItemCode group)
    : ItemSlot(), m_y(y), m_x(x), m_item_group(group)
{
    gw()->register_slot(this);
}

bool EncyclopediaSlot::undiscovered() {
    Item item = get_item();

    ItemHistory &history = gw()->game().history();
    return history.find(item.code) == end(history);
}

Item EncyclopediaSlot::get_item() {
    bool code_exists = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), [=](const ItemDefinition &def) {
        return def.code == item_code();
    }) != end(ITEM_DEFINITIONS);

    return code_exists ? Item(item_code()) : Item();
}

void EncyclopediaSlot::set_item(const Item &item) {
    bugcheck(EncyclopediaSlotSet, item.code, item.id, item.uses_left, item.intent);
}

void EncyclopediaSlot::refresh_pixmap() {
    Item item = get_item();

    if (undiscovered()) {
        m_item_label->setPixmap(Item::sil_pixmap_of(item.code));
    } else {
        m_item_label->setPixmap(Item::pixmap_of(item));
    }
}

std::optional<Item> EncyclopediaSlot::tooltip_item() {
    if (undiscovered()) {
        return std::optional<Item>();
    } else {
        return std::optional<Item>(get_item());
    }
}

std::optional<TooltipInfo> EncyclopediaSlot::tooltip_info() {
    if (undiscovered()) {
        return std::optional<TooltipInfo>({
            "???",
            "Undiscovered",
            "You havn't discovered this item yet.",
            QPixmap(":assets/img/items/sil/missing.png"),
            Item::def_of(item_code())->properties
        });
    } else {
        return std::optional<TooltipInfo>();
    }
}

void EncyclopediaSlot::mousePressEvent(QMouseEvent *) { }

void EncyclopediaSlot::dragEnterEvent(QDragEnterEvent *) { }

void EncyclopediaSlot::dropEvent(QDropEvent *) { }

ItemCode EncyclopediaSlot::item_code() {
    return (m_y * ENCYCLOPEDIA_GROUP_COLS + m_x) + m_item_group;
}
