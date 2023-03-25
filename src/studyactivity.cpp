#include "studyactivity.h"
#include "gamewindow.h"
#include "scanactivity.h"

StudyActivity::StudyActivity(AorInt ms_total, ItemId item_id)
    : TimedActivity(ms_total, ms_total), m_item_id(item_id) {}

void StudyActivity::complete() {
    gw()->game()->lore().add(lore_to_gain(), &AllCharacters::instance);

    for (auto &pair : gw()->game()->studied_items()) {
        for (ItemId &id : pair.second) {
            if (id == m_item_id) {
                id = EMPTY_ID;
            }
        }
    }

    gw()->game()->inventory().remove_item(m_item_id);
    gw()->refresh_ui();
    update_ui();
}

void StudyActivity::update_ui() {
    auto clamp = [](AorInt min, AorInt value, AorInt max) -> AorInt {
        return value < min ? min : (value > max ? max : value);
    };

    AorInt max_lore = gw()->game()->lore().max(&AllCharacters::instance);

    auto all_study_acts = gw()->game()->activities_of_type<StudyActivity>(Study);

    // We only want one activity to do updating.
    if (all_study_acts[0] != this) {
        return;
    }

    AorInt total_gain = std::accumulate(all_study_acts.begin(), all_study_acts.end(), 0, [](AorInt a, StudyActivity *act) {
        return a + act->lore_to_gain();
    });

    total_gain -= gw()->game()->activities_of_type<ScanActivity>(Scan).size() * LORE_PER_SCAN;

    gw()->window().lore_bar->setMaximum(max_lore);
    gw()->window().lore_bar->setValue(clamp(0, gw()->game()->lore().amount() + (total_gain * percent_complete()), max_lore));
}

AorInt StudyActivity::lore_to_gain() {
    Item item = gw()->game()->inventory().get_item(m_item_id);

    if (item.def()->type & Artifact) {
        return item.def()->properties[ItemLevel] * 20;
    } else if (item.def()->type & Tool) {
        return item.def()->properties[ItemLevel] * 15;
    } else {
        return item.def()->properties[ItemLevel] * 5;
    }
}

ItemDomain StudyActivity::type() {
    return Study;
}
