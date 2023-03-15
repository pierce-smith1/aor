#pragma once

#include "externalslot.h"

class MaterialSlot : public ExternalSlot {
public:
    MaterialSlot(size_t n);

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void accept_message(const SlotMessage &message) override;
    virtual void install() override;
    virtual void on_right_click(QMouseEvent *event) override;
    virtual ItemDomain type() override;

protected:
    virtual Item my_item() override;
};
