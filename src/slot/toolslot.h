#pragma once

#include "externalslot.h"

class ToolSlot : public ExternalSlot {
public:
    ToolSlot(ItemDomain tool_type);

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void accept_message(const SlotMessage &message) override;
    virtual void install() override;
    virtual ItemDomain type() override;

protected:
    virtual Item my_item() override;

    ItemDomain m_tool_type;
};
