#include "externalslot.h"
#include "items.h"

ExternalSlot::ExternalSlot(LKGameWindow *game, ItemDomain type, int n)
    : ItemSlot(game), item_slot_type(type), n(n), m_held_item_id(EMPTY_ID)
{
    setObjectName(make_internal_name("external_slot", type, n));
    m_item_layout->setObjectName(make_internal_name("external_layout", type, n));
    m_item_label->setObjectName(make_internal_name("external_label", type, n));

    game->register_slot_name(objectName());
}

Item ExternalSlot::get_item() {
    if (m_held_item_id == EMPTY_ID) {
        return Item();
    }

    return m_game_window->game().inventory().get_item(m_held_item_id);
}

void ExternalSlot::set_item(const Item &item) {
    m_held_item_id = item.id;
    m_game_window->selected_char().external_items()[type()][n] = get_item().id;
}

ItemDomain ExternalSlot::type() {
    return item_slot_type;
}

void ExternalSlot::refresh_pixmap() {
    // After a reload, we need to refresh the item behind this external slot
    ItemId id;
    switch (type()) {
        default:
        case Ordinary: {
            ItemSlot::refresh_pixmap();
            return;
        }
        case Material:
        case Offering:
        case KeyOffering:
        case Artifact: {
            id = m_game_window->selected_char().external_items().at(type())[n];
            break;
        }
    }
    m_held_item_id = id;

    if (type() == KeyOffering) {
        setFrameShape(NoFrame);
        setStyleSheet("border: 1px solid #ffcc66; border-radius: 5px");
    }

    ItemSlot::refresh_pixmap();
}

void ExternalSlot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data {event->mimeData()};
    if (!data->hasFormat("text/plain")) {
        return;
    }

    if (m_game_window->selected_char().activity_ongoing()) {
        return;
    }

    QString source_slot_name {event->mimeData()->text()};
    ItemSlot *source_slot {m_game_window->findChild<ItemSlot *>(source_slot_name)};
    Item dropped_item {source_slot->get_item()};
    ItemType dropped_type {dropped_item.def()->type};

    switch (type()) {
        case KeyOffering:
        case Offering: {
            if (!(dropped_type & Rune)) { event->acceptProposedAction(); } break;
        }
        case Portrait: {
            if (dropped_type & Consumable) { event->acceptProposedAction(); } break;
        }
        default: {
            if (dropped_type & item_slot_type) { event->acceptProposedAction(); } break;
        }
    }
}

void ExternalSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name {event->mimeData()->text()};
    ItemSlot *source_slot {m_game_window->findChild<ItemSlot *>(source_slot_name)};

    Item source_item {source_slot->get_item()};
    Item this_item {get_item()};

    set_item(source_item);
    if (source_slot->type() != Ordinary) {
        source_slot->set_item(this_item);
    }

    m_game_window->game().inventory().get_item_ref(source_item.id).intent = type();
    m_game_window->game().inventory().get_item_ref(source_item.id).intent_holder = m_game_window->selected_char().id();

    source_slot->refresh_pixmap();
    refresh_pixmap();
    m_game_window->refresh_ui_buttons();
}

void ExternalSlot::insert_external_slots(LKGameWindow &window) {
    QGridLayout *smith_layout {window.window().smith_layout};
    QGridLayout *prayer_layout {window.window().prayer_layout};
    QLayout *aritfact_layout {window.window().artifact_box->layout()};

    for (int i {0}; i < SMITHING_SLOTS; i++) {
        smith_layout->addWidget(
            new ExternalSlot(&window, Material, i),
            i / SMITHING_SLOTS_PER_ROW + 1,
            i % SMITHING_SLOTS_PER_ROW
        );
    }

    for (int i {0}; i < PRAYER_SLOTS; i++) {
        int y = i / PRAYER_SLOTS_PER_ROW;
        int x = i % PRAYER_SLOTS_PER_ROW;

        ExternalSlot *slot;
        if (y == PRAYER_KEY_SLOT_Y && x == PRAYER_KEY_SLOT_X) {
            slot = new ExternalSlot(&window, KeyOffering, i);
        } else {
            slot = new ExternalSlot(&window, Offering, i);
        }

        prayer_layout->addWidget(slot, y + 1, x);
    }

    for (int i {0}; i < ARTIFACT_SLOTS; i++) {
        aritfact_layout->addWidget(new ExternalSlot(&window, Artifact, i));
    }
}

ToolSlot::ToolSlot(LKGameWindow *game, ItemDomain type)
    : ExternalSlot(game, type, 0), m_tool_slot_type(type)
{
    setMinimumSize(QSize(0, 80));
    setMaximumSize(QSize(10000, 80));
}

void ToolSlot::set_item(const Item &item) {
    m_game_window->selected_char().tools()[get_tool_slot_type()] = item.id;
}

void ToolSlot::refresh_pixmap() {
    m_held_item_id = m_game_window->selected_char().tools()[get_tool_slot_type()];
    ExternalSlot::refresh_pixmap();
}

ItemDomain ToolSlot::get_tool_slot_type() {
    return m_tool_slot_type;
}

void ToolSlot::insert_tool_slots(LKGameWindow &window) {
    window.window().smith_layout->addWidget(new ToolSlot(&window, SmithingTool), 0, 0, 1, 3);
    window.window().exploring_layout->addWidget(new ToolSlot(&window, ForagingTool), 0, 0);
    window.window().exploring_layout->addWidget(new ToolSlot(&window, MiningTool), 2, 0);
    window.window().prayer_layout->addWidget(new ToolSlot(&window, PrayerTool), 0, 0, 1, 3);
}

PortraitSlot::PortraitSlot(LKGameWindow *game)
    : ExternalSlot(game, Portrait, 0)
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
    window.window().player_layout->addWidget(new PortraitSlot(&window));
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
    ItemSlot *source_slot {m_game_window->findChild<ItemSlot *>(source_slot_name)};
    Item item {source_slot->get_item()};

    if (source_slot->type() != Ordinary) {
        return;
    }

    if (item.def()->type & Consumable) {
        m_game_window->game().inventory().get_item_ref(item.id).intent = Consumable;
        m_game_window->game().inventory().get_item_ref(item.id).intent_holder = m_game_window->selected_char().id();
        m_game_window->start_activity(CharacterActivity(Eating, 60 * 1000));

        source_slot->refresh_pixmap();
    }
}
