// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "externalslot.h"

class ToolSlot : public ExternalSlot {
public:
    ToolSlot(ItemDomain tool_type);

    ItemDomain type();

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void install() override;

protected:
    virtual ItemId &my_item_id() override;

    ItemDomain m_tool_type;
};
