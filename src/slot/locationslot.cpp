#include "locationslot.h"

LocationSlot::LocationSlot(const LocationDefinition &def, QWidget *parent)
    : m_location_def(def)
{
    setParent(parent);
    setMinimumSize(32, 32);
    setMaximumSize(32, 32);
}

bool LocationSlot::do_hovering() {
    return m_location_def.id != NOWHERE;
}

std::optional<TooltipInfo> LocationSlot::tooltip_info() {
    return std::optional<TooltipInfo>({
        QString("<b>%1</b>").arg(m_location_def.display_name),
        QString("Location"),
        QString("A place"),
        Item::pixmap_of("byteberry"),
        {},
        std::optional<QColor>()
    });
}

void LocationSlot::refresh() {
    Slot::refresh();

    if (m_location_def.id == gw()->game()->current_location_id()) {
        setStyleSheet("*[slot=\"true\"] { border: 2px solid #888; border-radius: 3px; background-color: white; }");
    } else {
        setStyleSheet("*[slot=\"true\"] { border: 1px solid #bbb; border-radius: 3px; background-color: white; }");
    }
}
