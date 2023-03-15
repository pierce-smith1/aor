#include "toolslot.h"

ToolSlot::ToolSlot(ItemDomain tool_type)
    : ExternalSlot(0), m_tool_type(tool_type)
{
    setMinimumSize(QSize(0, 80));
    setMaximumSize(QSize(10000, 80));
}

bool ToolSlot::will_accept_drop(const SlotMessage &message) {
    Item item = get_item(message);
    return Item::def_of(item)->type & m_tool_type;
}

void ToolSlot::accept_message(const SlotMessage &message) {
    switch (message.type) {
        case SlotUserDrop: {
            message.source->accept_message(SlotMessage(SlotForgetItem, my_item(), this));
            accept_message(SlotMessage(SlotSetItem, message, this));
            break;
        } case SlotSetItem: {
            gw()->selected_char().tools()[m_tool_type] = get_item(message).id;
            break;
        } case SlotForgetItem: {
            gw()->selected_char().tools()[m_tool_type] = EMPTY_ID;
            break;
        } default: {}
    }
}

ItemDomain ToolSlot::type() {
    return m_tool_type;
}

void ToolSlot::install() {
    switch (m_tool_type) {
        case SmithingTool: {
            gw()->window().smith_layout->addWidget(this, 0, 0, 1, 5);
            break;
        }
        case ForagingTool: {
            gw()->window().exploring_layout->addWidget(this, 0, 0);
            break;
        }
        case MiningTool: {
            gw()->window().exploring_layout->addWidget(this, 2, 0);
            break;
        }
        default: {
            bugcheck(UnimplementedSlotInstall, "tool", m_tool_type);
        }
    }
}

Item ToolSlot::my_item() {
    return inventory().get_item(gw()->selected_char().tool_id(m_tool_type));
}
