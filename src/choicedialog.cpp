// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "choicedialog.h"
#include "items.h"
#include "gamewindow.h"

ChoiceDialog::ChoiceDialog(const WeightedVector<Item> &items) {
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel(this);
    label->setText("<i>Make your choice...</i>");
    label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(label);

    QWidget *slot_area = new QWidget();
    QHBoxLayout *slot_layout = new QHBoxLayout();

    for (size_t n = 0; n < items.size(); n++) {
        const Item &item = items[n].first;
        slot_layout->addWidget(new ChoiceSlot(item.code, this, n));
    }
    slot_area->setLayout(slot_layout);
    layout->addWidget(slot_area);

    setLayout(layout);

    setMaximumSize(QSize(9999999, 56));
    setMinimumSize(QSize(0, 120));
}
