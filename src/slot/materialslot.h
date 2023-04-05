// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "externalslot.h"

class MaterialSlot : public ExternalSlot {
public:
    MaterialSlot(size_t n);

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void install() override;

protected:
    virtual ItemId &my_item_id() override;
};
