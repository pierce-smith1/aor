#include "encyclopedia.h"

Encyclopedia::Encyclopedia(LKGameWindow *game_window)
    : m_game_window(game_window), m_widget()
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
                box_layout->addWidget(new EncyclopediaSlot(game_window, y, x, group), y, x);
            }
        }
    }
}

EncyclopediaSlot::EncyclopediaSlot(LKGameWindow *game_window, int y, int x, ItemCode group)
    : ItemSlot(game_window), m_y(y), m_x(x), m_item_group(group)
{
    game_window->register_slot(this);
}

bool EncyclopediaSlot::undiscovered() {
    Item item = get_item();

    ItemHistory &history = m_game_window->game().history();
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

void EncyclopediaSlot::enterEvent(QEvent *event) {
    Item item = get_item();

    if (item.id == EMPTY_ID) {
        return;
    }

    QEnterEvent *enter_event = (QEnterEvent *) event;
    m_game_window->tooltip().move(enter_event->globalPos());

    if (undiscovered()) {
        m_game_window->tooltip().set_text(m_game_window->game().tooltip_text_for(Item()));
        m_game_window->tooltip().set_resources(Item());
        m_game_window->tooltip().widget.item_image->setPixmap(QPixmap("assets/img/items/sil/missing.png"));
    } else {
        m_game_window->tooltip().set_text(m_game_window->game().tooltip_text_for(item));
        m_game_window->tooltip().set_resources(item);
        m_game_window->tooltip().widget.item_image->setPixmap(Item::pixmap_of(item));
    }

    m_game_window->tooltip().adjustSize();
    m_game_window->tooltip().show();
}

void EncyclopediaSlot::mousePressEvent(QMouseEvent *) { }

void EncyclopediaSlot::dragEnterEvent(QDragEnterEvent *) { }

void EncyclopediaSlot::dropEvent(QDropEvent *) { }
