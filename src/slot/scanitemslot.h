#pragma once

#include "externalslot.h"

class ScanItemSlot : public ExternalSlot {
public:
    ScanItemSlot();

    virtual void install() override;
    virtual void on_right_click(QMouseEvent *event) override;

protected:
    virtual ItemId &my_item_id() override;
};
