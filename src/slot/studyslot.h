#pragma once

#include "externalslot.h"

class StudySlot : public ExternalSlot {
public:
    StudySlot(ItemDomain type, size_t n);

    bool will_accept_drop(const SlotMessage &message) override;
    void accept_message(const SlotMessage &message) override;
    bool is_draggable() override;
    void install() override;

protected:
    ItemId &my_item_id() override;

    ItemDomain m_type;
};
