// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "itemslot.h"

class QueuedActivitySlot : public Slot {
public:
    QueuedActivitySlot(size_t n);

    virtual bool do_hovering() override;
    std::optional<TooltipInfo> tooltip_info() override;

    virtual QPixmap pixmap() override;
    virtual void on_right_click(QMouseEvent *event) override;
    virtual void install() override;

private:
    TimedActivity &activity();

    size_t m_n;
};
