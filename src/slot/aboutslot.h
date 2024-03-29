// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "slot.h"

class AboutSlot : public Slot {
public:
    AboutSlot() = default;

    virtual bool do_hovering() override;
    virtual std::optional<TooltipInfo> tooltip_info() override;

    virtual QPixmap pixmap() override;
};
