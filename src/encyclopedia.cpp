#include "encyclopedia.h"

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
                box_layout->addWidget(new EncyclopediaSlot(y, x, group), y, x);
            }
        }
    }
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
    ItemCode code = (m_y * ENCYCLOPEDIA_GROUP_COLS + m_x) + m_item_group;

    bool code_exists = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), [=](const ItemDefinition &def) {
        return def.code == code;
    }) != end(ITEM_DEFINITIONS);

    return code_exists ? Item(code) : Item();
}

void EncyclopediaSlot::set_item(const Item &) {
    qFatal("Tried to set item of encyclopedia slot");
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
            QPixmap(":assets/img/items/sil/missing.png")
        });
    } else {
        return std::optional<TooltipInfo>();
    }
}

void EncyclopediaSlot::mousePressEvent(QMouseEvent *) { }

void EncyclopediaSlot::dragEnterEvent(QDragEnterEvent *) { }

void EncyclopediaSlot::dropEvent(QDropEvent *) { }
