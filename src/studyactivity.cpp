#include "studyactivity.h"
#include "gamewindow.h"

StudyActivity::StudyActivity(AorInt ms_total, ItemId item_id)
    : TimedActivity(ms_total, ms_total), m_item_id(item_id) {}

void StudyActivity::complete() {
    gw()->game()->lore() += lore_to_gain();

    for (auto &pair : gw()->game()->studied_items()) {
        for (ItemId &id : pair.second) {
            if (id == m_item_id) {
                id = EMPTY_ID;
            }
        }
    }

    update_ui();

    gw()->game()->inventory().remove_item(m_item_id);
    gw()->refresh_ui();
}

void StudyActivity::update_ui() {
    auto all_study_acts = gw()->game()->activities_of_type<StudyActivity>(Study);

    // We only want one activity to do updating.
    if (all_study_acts[0] != this) {
        return;
    }

    double total_gain = std::accumulate(all_study_acts.begin(), all_study_acts.end(), 0.0, [=](AorInt a, StudyActivity *act) {
        return a + (isActive() ? (act->lore_to_gain() * (act->percent_complete() / 100.0)) : 0.0);
    });

    gw()->window().lore_label->setText(QString("<b>%1</b>")
        .arg(gw()->game()->lore() + total_gain));

    gw()->game()->map().reveal_progress() = 1 + (gw()->game()->lore() + total_gain) / LORE_PER_SCAN;
    gw()->refresh_map();
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
