#include "aboutslot.h"
#include "../main.h"

bool AboutSlot::do_hovering() {
    return true;
}

std::optional<TooltipInfo> AboutSlot::tooltip_info() {
    return std::optional<TooltipInfo>({
        "<b>Aegis of Rhodon</b>",
        QString("Level b%1.%2.%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(PATCH_VERSION),
        "<i>Be still, Rhodon.</i>"
        "<i>https://doughbyte.com</i>",
        Item::pixmap_of("welchian_rune"),
        {},
        std::optional<QColor>()
    });
}

QPixmap AboutSlot::pixmap() {
    return Item::pixmap_of("welchian_rune");
}
