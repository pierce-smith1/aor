#include "scanitemslot.h"

ScanItemSlot::ScanItemSlot()
    : ExternalSlot(0) {}

void ScanItemSlot::install() {
    dynamic_cast<QGridLayout *>(gw()->window().scan_sac_box->layout())->addWidget(this, 0, 0);
}

void ScanItemSlot::on_right_click(QMouseEvent *event) {
    if (gw()->game()->can_scan()) {
        ExternalSlot::on_right_click(event);
    }
}

ItemId &ScanItemSlot::my_item_id() {
    return gw()->game()->scan_item_id();
}
