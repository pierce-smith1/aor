#pragma once

#include "externalslot.h"

class StudySlot : public ExternalSlot {
public:
    StudySlot(ItemDomain type, size_t n);

    bool will_accept_drop(const SlotMessage &message) override;
    bool is_draggable() override;
    void install() override;

protected:
    ItemDomain m_type;
};
