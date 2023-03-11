#pragma once

#include "externalslot.h"

class MaterialSlot : public ExternalSlot {
public:
    MaterialSlot(size_t n);

    virtual bool will_accept_drop(const DropPayload &payload) override;
    virtual void accept_drop(const DropPayload &payload) override;
    virtual void after_dropped_elsewhere(const DropPayload &payload) override;
    virtual void install() override;
    virtual void on_right_click(QMouseEvent *event) override;
    virtual ItemDomain type() override;

protected:
    virtual Item my_item() override;
};
