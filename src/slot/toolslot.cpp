// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "toolslot.h"

ToolSlot::ToolSlot(ItemDomain tool_type)
    : ExternalSlot(0), m_tool_type(tool_type)
{
    make_wide();
}

bool ToolSlot::will_accept_drop(const SlotMessage &message) {
    if (!std::holds_alternative<Item>(message)) {
        return false;
    }

    if (gw()->selected_char().activity().active) {
        return false;
    }

    return Item::def_of(get_item(message))->type & m_tool_type;
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

ItemId &ToolSlot::my_item_id() {
    return gw()->selected_char().tools()[m_tool_type];
}
