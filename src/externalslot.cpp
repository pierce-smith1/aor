#include "externalslot.h"
#include "items.h"
#include "foreigntradeslot.h"
#include "smithingresultslot.h"
#include "die.h"

ExternalSlot::ExternalSlot(ItemDomain type, int n)
    : ItemSlot(), item_slot_type(type), n(n)
{
    setObjectName(make_internal_name("external_slot", type, n));
    m_item_layout->setObjectName(make_internal_name("external_layout", type, n));
    m_item_label->setObjectName(make_internal_name("external_label", type, n));

    gw()->register_slot(this);
}

Item ExternalSlot::get_item() {
    if (held_item_id() == EMPTY_ID) {
        return Item();
    }

    return gw()->game().inventory().get_item(held_item_id());
}

void ExternalSlot::set_item(const Item &item) {
    gw()->selected_char().external_items()[type()][n] = item.id;

    if (type() == Offering) {
        gw()->connection().offer_changed(item, n);
        gw()->game().trade_offer()[n] = item.id;
    }
}

ItemDomain ExternalSlot::type() {
    return item_slot_type;
}

void ExternalSlot::refresh_pixmap() {
    ItemSlot::refresh_pixmap();
}

ItemId ExternalSlot::held_item_id() {
    if (type() == Offering) {
        return gw()->game().trade_offer()[n];
    } else {
        return gw()->selected_char().external_items().at(type())[n];
    }
}

void ExternalSlot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data = event->mimeData();
    if (!data->hasFormat("text/plain")) {
        return;
    }

    if (gw()->selected_char().activity().ongoing()) {
        return;
    }

    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = gw()->findChild<ItemSlot *>(source_slot_name);
    Item dropped_item = source_slot->get_item();
    ItemType dropped_type = dropped_item.def()->type;

    if (source_slot->type() == Explorer) {
        return;
    }

    switch (type()) {
        case Offering: {
            if (gw()->game().trade_partner() == NOBODY
                && !(dropped_item.def()->type & Untradeable)
            ) {
                event->acceptProposedAction();
            }
            break;
        }
        case Portrait: {
            event->acceptProposedAction();
            break;
        }
        case Material: {
            if (!Item::has_resource_value(dropped_item.code)) {
                break;
            }

            bool tool_supports = true;
            ItemProperties current_resources = gw()->selected_char().total_material_resources();
            Item tool = gw()->game().inventory().get_item(gw()->selected_char().tool_id(Smithing));
            Item::for_each_resource_type([&](ItemProperty, ItemProperty max_prop, ItemProperty resource_prop) {
                int resource_ceiling = current_resources[resource_prop] + dropped_item.def()->properties[resource_prop];
                int tool_max = tool.def()->properties[max_prop];
                if (tool_max == 0) {
                    tool_max = BASE_MAX_RESOURCE;
                }

                if (resource_ceiling > tool_max) {
                    tool_supports = false;
                }
            });

            if (tool_supports) {
                event->acceptProposedAction();
            }

            break;
        }
        default: {
            if (dropped_type & item_slot_type) {
                event->acceptProposedAction();
            }
            break;
        }
    }
}

void ExternalSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = gw()->findChild<ItemSlot *>(source_slot_name);

    Item source_item = source_slot->get_item();
    Item this_item = get_item();

    set_item(source_item);
    if (source_slot->type() != Ordinary) {
        source_slot->set_item(this_item);
    }

    gw()->game().inventory().get_item_ref(source_item.id).intent = type();
    gw()->game().inventory().get_item_ref(source_item.id).intent_holder = gw()->selected_char().id();

    gw()->refresh_ui();
}

void ExternalSlot::insert_external_slots() {
    for (int i = 0; i < SMITHING_SLOTS; i++) {
        gw()->window().smith_layout->addWidget(
            new ExternalSlot(Material, i),
            i / SMITHING_SLOTS_PER_ROW + 1,
            i % SMITHING_SLOTS_PER_ROW
        );
    }
    gw()->window().smith_button_layout->addWidget(new SmithingResultSlot());

    for (int i = 0; i < TRADE_SLOTS; i++) {
        gw()->window().trade_slot_layout->addWidget(new ExternalSlot(Offering, i));
        gw()->window().foreign_trade_slot_layout->addWidget(new ForeignTradeSlot(i));
    }

    for (int i = 0; i < ARTIFACT_SLOTS; i++) {
        gw()->window().artifact_layout->addWidget(new ExternalSlot(Artifact, i));
    }
}

ToolSlot::ToolSlot(ItemDomain type)
    : ExternalSlot(type, 0), m_tool_slot_type(type)
{
    setMinimumSize(QSize(0, 80));
    setMaximumSize(QSize(10000, 80));
}

void ToolSlot::set_item(const Item &item) {
    gw()->selected_char().tools()[get_tool_slot_type()] = item.id;
}

void ToolSlot::refresh_pixmap() {
    ExternalSlot::refresh_pixmap();
}

ItemId ToolSlot::held_item_id() {
    return gw()->selected_char().tools()[get_tool_slot_type()];
}

ItemDomain ToolSlot::get_tool_slot_type() {
    return m_tool_slot_type;
}

void ToolSlot::insert_tool_slots() {
    gw()->window().smith_layout->addWidget(new ToolSlot(SmithingTool), 0, 0, 1, 5);
    gw()->window().exploring_layout->addWidget(new ToolSlot(ForagingTool), 0, 0);
    gw()->window().exploring_layout->addWidget(new ToolSlot(MiningTool), 3, 0);
}

PortraitSlot::PortraitSlot()
    : ExternalSlot(Portrait, 0)
{
    setMinimumSize(QSize(160, 230));
    setMaximumSize(QSize(160, 230));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

Item PortraitSlot::get_item() {
    bugcheck(PortraitSlotGet);
    return Item();
}

void PortraitSlot::set_item(const Item &item) {
    bugcheck(PortraitSlotSet, item.code, item.id, item.uses_left, item.intent);
}

void PortraitSlot::refresh_pixmap() {
}

void PortraitSlot::insert_portrait_slot() {
    gw()->window().player_layout->addWidget(new PortraitSlot());
}

void PortraitSlot::enterEvent(QEvent *) { }

void PortraitSlot::mouseMoveEvent(QMouseEvent *) { }

void PortraitSlot::mousePressEvent(QMouseEvent *) { }

void PortraitSlot::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = gw()->findChild<ItemSlot *>(source_slot_name);
    Item item = source_slot->get_item();

    if (source_slot->type() != Ordinary) {
        return;
    }

    if (item.def()->type & Consumable) {
        gw()->game().inventory().get_item_ref(item.id).intent = Consumable;
        gw()->game().inventory().get_item_ref(item.id).intent_holder = gw()->selected_char().id();
        gw()->selected_char().start_activity(Eating);

        source_slot->refresh_pixmap();
    } else {
        gw()->game().inventory().get_item_ref(item.id).intent = Defiling;
        gw()->game().inventory().get_item_ref(item.id).intent_holder = gw()->selected_char().id();
        gw()->selected_char().start_activity(Defiling);

        source_slot->refresh_pixmap();
    }
}
