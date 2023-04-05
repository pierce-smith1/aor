// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "aboutslot.h"
#include "../main.h"

bool AboutSlot::do_hovering() {
    return true;
}

std::optional<TooltipInfo> AboutSlot::tooltip_info() {
    return std::optional<TooltipInfo>({
        "<b>Aegis of Rhodon</b>",
        QString("Level b%1.%2.%3").arg(AOR_MAJOR_VERSION).arg(AOR_MINOR_VERSION).arg(AOR_PATCH_VERSION),
        "<i>Be still, Rhodon.</i><br>"
        "<i>https://doughbyte.com</i>",
        Item::pixmap_of("welchian_rune"),
        {},
        std::optional<QColor>()
    });
}

QPixmap AboutSlot::pixmap() {
    return Item::pixmap_of("welchian_rune");
}
