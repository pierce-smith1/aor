// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "externalslot.h"

class TradeSlot : public ExternalSlot {
public:
    TradeSlot(size_t n);

    virtual void install() override;
    virtual bool will_accept_drop(const SlotMessage &) override;
    virtual void accept_message(const SlotMessage &) override;

protected:
    virtual ItemId &my_item_id() override;
};
