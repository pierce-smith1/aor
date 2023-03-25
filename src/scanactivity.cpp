#include "scanactivity.h"
#include "gamewindow.h"
#include "studyactivity.h"

ScanActivity::ScanActivity(AorInt ms_total)
    : ScanActivity(ms_total, ms_total) {}

ScanActivity::ScanActivity(AorInt ms_total, AorInt ms_left)
     : TimedActivity(ms_total, ms_left) {}

void ScanActivity::complete() {
    gw()->game()->map().scan_from(gw()->game()->current_location_id(), 1);
    gw()->game()->lore().add(-LORE_PER_SCAN, &AllCharacters::instance);

    gw()->refresh_ui();
    update_ui();
}

void ScanActivity::update_ui() {
    auto clamp = [](AorInt min, AorInt value, AorInt max) -> AorInt {
        return value < min ? min : (value > max ? max : value);
    };

    gw()->window().map_progress_bar->setMaximum(100);
    gw()->window().map_progress_bar->setValue(percent_complete() * 100);

    // Don't update the lore bar if there are study activities; they will take care of it.
    if (!gw()->game()->activities_of_type<StudyActivity>(Study).empty()) {
        return;
    }

    AorInt max_lore = gw()->game()->lore().max(&AllCharacters::instance);
    gw()->window().lore_bar->setMaximum(max_lore);
    gw()->window().lore_bar->setValue(clamp(0, gw()->game()->lore().amount() + (-LORE_PER_SCAN * percent_complete()), max_lore));
}

ItemDomain ScanActivity::type() {
    return Scan;
}
