// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "slot.h"

class PortraitSlot : public Slot {
public:
    PortraitSlot();

    virtual void refresh() override;
    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void accept_message(const SlotMessage &message) override;
    virtual QPixmap pixmap() override;
    virtual void install() override;

private:
    QWidget *m_container;
};
