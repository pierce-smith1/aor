#pragma once

#include "externalslot.h"

class ScanItemSlot : public ExternalSlot {
public:
    ScanItemSlot();

    virtual void install() override;

protected:
    virtual ItemId &my_item_id() override;
};
