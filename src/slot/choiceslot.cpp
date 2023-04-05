// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "choiceslot.h"
#include "../choicedialog.h"

ChoiceSlot::ChoiceSlot(ItemCode code, ChoiceDialog *parent, size_t n)
    : m_code(code), m_dialog(parent), m_n(n)
{
    gw()->unregister_slot(this);
    setParent(parent);
    refresh();
}

bool ChoiceSlot::do_hovering() {
    return true;
}

std::optional<Item> ChoiceSlot::tooltip_item() {
    return std::optional<Item>(m_code);
}

QPixmap ChoiceSlot::pixmap() {
    return Item::pixmap_of(m_code);
}

void ChoiceSlot::on_left_click(QMouseEvent *) {
    m_dialog->done(m_n);
}
