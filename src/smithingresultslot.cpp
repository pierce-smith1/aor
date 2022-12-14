#include "smithingresultslot.h"
#include "gamewindow.h"

SmithingResultSlot::SmithingResultSlot()
    : ItemSlot()
{
    setObjectName("smithing_result_slot");
    gw()->register_slot(this);
}

Item SmithingResultSlot::get_item() {
    return Item(m_code);
}

void SmithingResultSlot::set_item(const Item &item) {
    m_code = item.code;
}

void SmithingResultSlot::refresh_pixmap() {
    m_code = gw()->selected_char().smithing_result();

    ItemSlot::refresh_pixmap();
}

void SmithingResultSlot::mousePressEvent(QMouseEvent *) { }

void SmithingResultSlot::dragEnterEvent(QDragEnterEvent *) { }
