#include "scanactivity.h"
#include "gamewindow.h"

ScanActivity::ScanActivity(AorInt ms_total)
    : ScanActivity(ms_total, ms_total) {}

ScanActivity::ScanActivity(AorInt ms_total, AorInt ms_left)
     : TimedActivity(ms_total, ms_left) {}

void ScanActivity::complete() {
    AorUInt sac_level = gw()->game()->inventory().get_item(gw()->game()->scan_item_id()).def()->properties[ItemLevel];
    gw()->game()->map().scan_from(gw()->game()->current_location_id(), sac_level);

    gw()->game()->inventory().remove_item(gw()->game()->scan_item_id());
    gw()->game()->scan_item_id() = EMPTY_ID;

    gw()->refresh_ui();
}

void ScanActivity::update_ui() {
    gw()->window().map_progress_bar->setMaximum(100);
    gw()->window().map_progress_bar->setValue(percent_complete() * 100);
}

ItemDomain ScanActivity::type() {
    return Scan;
}
