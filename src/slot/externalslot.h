// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "itemslot.h"

class ExternalSlot : public ItemSlot {
public:
    ExternalSlot(size_t n);

    virtual void accept_message(const SlotMessage &message) override;
    virtual void on_right_click(QMouseEvent *event) override;

protected:
    virtual Item my_item() override;
    virtual ItemId &my_item_id();

    size_t m_n;
};
