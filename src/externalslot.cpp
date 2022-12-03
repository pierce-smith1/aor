#include "externalslot.h"
#include "items.h"
#include "foreigntradeslot.h"

ExternalSlot::ExternalSlot(LKGameWindow *game, ItemDomain type, int n)
    : ItemSlot(game), item_slot_type(type), n(n)
{
    setObjectName(make_internal_name("external_slot", type, n));
    m_item_layout->setObjectName(make_internal_name("external_layout", type, n));
    m_item_label->setObjectName(make_internal_name("external_label", type, n));

    game->register_slot_name(objectName());
}

Item ExternalSlot::get_item() {
    if (held_item_id() == EMPTY_ID) {
        return Item();
    }

    return m_game_window->game().inventory().get_item(held_item_id());
}

void ExternalSlot::set_item(const Item &item) {
    m_game_window->selected_char().external_items()[type()][n] = item.id;

    if (type() == Offering) {
        m_game_window->connection().set_offering(n, item);
    }
}

ItemDomain ExternalSlot::type() {
    return item_slot_type;
}

void ExternalSlot::refresh_pixmap() {
    ItemSlot::refresh_pixmap();
}

ItemId ExternalSlot::held_item_id() {
    return m_game_window->selected_char().external_items().at(type())[n];
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
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = m_game_window->findChild<ItemSlot *>(source_slot_name);

    Item source_item = source_slot->get_item();
    Item this_item = get_item();

    set_item(source_item);
    if (source_slot->type() != Ordinary) {
        source_slot->set_item(this_item);
    }

    m_game_window->game().inventory().get_item_ref(source_item.id).intent = type();
    m_game_window->game().inventory().get_item_ref(source_item.id).intent_holder = m_game_window->selected_char().id();

    m_game_window->refresh_ui();
}

void ExternalSlot::insert_external_slots(LKGameWindow &window) {
    QGridLayout *smith_layout = window.window().smith_layout;
    QHBoxLayout *trade_layout = window.window().trade_slot_layout;
    QHBoxLayout *foreign_trade_layout = window.window().foreign_trade_slot_layout;
    QLayout *aritfact_layout = window.window().artifact_box->layout();

    for (int i = 0; i < SMITHING_SLOTS; i++) {
        smith_layout->addWidget(
            new ExternalSlot(&window, Material, i),
            i / SMITHING_SLOTS_PER_ROW + 1,
            i % SMITHING_SLOTS_PER_ROW
        );
    }

    for (int i = 0; i < TRADE_SLOTS; i++) {
        trade_layout->addWidget(new ExternalSlot(&window, Offering, i));
        foreign_trade_layout->addWidget(new ForeignTradeSlot(&window, i));
    }

    for (int i = 0; i < ARTIFACT_SLOTS; i++) {
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
    ExternalSlot::refresh_pixmap();
}

ItemId ToolSlot::held_item_id() {
    return m_game_window->selected_char().tools()[get_tool_slot_type()];
}

ItemDomain ToolSlot::get_tool_slot_type() {
    return m_tool_slot_type;
}

void ToolSlot::insert_tool_slots(LKGameWindow &window) {
    window.window().smith_layout->addWidget(new ToolSlot(&window, SmithingTool), 0, 0, 1, 3);
    window.window().exploring_layout->addWidget(new ToolSlot(&window, ForagingTool), 0, 0);
    window.window().exploring_layout->addWidget(new ToolSlot(&window, MiningTool), 2, 0);
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
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = m_game_window->findChild<ItemSlot *>(source_slot_name);
    Item item = source_slot->get_item();

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
