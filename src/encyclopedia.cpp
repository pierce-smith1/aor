#include "encyclopedia.h"
#include "die.h"

Encyclopedia::Encyclopedia()
    : m_widget()
{
    m_widget.setupUi(this);

    for (ItemCode group = CT_CONSUMABLE; group.n <= CT_ARTIFACT; group.n <<= 1) {
        QGroupBox *group_box = new QGroupBox(this);
        QGridLayout *box_layout = new QGridLayout;
        group_box->setLayout(box_layout);

        switch (group) {
            case CT_CONSUMABLE: { group_box->setTitle("Consumables"); break; }
            case CT_MATERIAL:   { group_box->setTitle("Materials"); break; }
            case CT_TOOL:       { group_box->setTitle("Tools"); break; }
            case CT_ARTIFACT:   { group_box->setTitle("Artifacts"); break; }
        }
        m_widget.encyclopedia_slots->layout()->addWidget(group_box);

        for (int y = 0; y < ENCYCLOPEDIA_GROUP_ROWS; y++) {
            for (int x = 0; x < ENCYCLOPEDIA_GROUP_COLS; x++) {
                EncyclopediaSlot *slot = new EncyclopediaSlot(y, x, group);
                box_layout->addWidget(slot, y, x);

                if (slot->valid()) {
                    m_total_items++;
                }
            }
        }
    }
}

void Encyclopedia::refresh() {
    m_widget.progress_bar->setMaximum(m_total_items);

    int discovered = 0;
    for (EncyclopediaSlot *slot : findChildren<EncyclopediaSlot *>()) {
        if (!slot->undiscovered()) {
            discovered++;
        }
    }

    m_widget.progress_bar->setValue(discovered);
}
