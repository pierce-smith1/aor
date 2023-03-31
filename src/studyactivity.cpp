#include "studyactivity.h"
#include "gamewindow.h"

void StudyActivity::complete(const TimedActivity &activity) {
    gw()->game()->lore() += lore_to_gain(activity);

    for (auto &pair : gw()->game()->studied_items()) {
        for (ItemId &id : pair.second) {
            if (id == activity.owned_item_ids.at(0)) {
                id = EMPTY_ID;
            }
        }
    }

    gw()->game()->inventory().remove_item(activity.owned_item_ids.at(0));
    gw()->refresh_ui();
}

void StudyActivity::update_ui(const TimedActivity &activity) {
    auto all_study_acts = gw()->game()->activities_of_type(Study);

    // We only want one activity to do updating.
    if (!all_study_acts.empty() && all_study_acts[0].id != activity.id) {
        return;
    }

    double total_gain = std::accumulate(all_study_acts.begin(), all_study_acts.end(), 0.0, [=](AorInt a, TimedActivity &act) {
        return a + (act.active ? (lore_to_gain(act) * (act.percent_complete() / 100.0)) : 0.0);
    });

    gw()->window().lore_label->setText(QString("<b>%1</b>")
        .arg(gw()->game()->lore() + total_gain));

    gw()->game()->map().reveal_progress() = 1 + (gw()->game()->lore() + total_gain) / LORE_PER_SCAN;
    gw()->refresh_map();
}

AorInt StudyActivity::lore_to_gain(const TimedActivity &activity) {
    Item item = gw()->game()->inventory().get_item(activity.owned_item_ids.at(0));

    AorInt gain = 0;
    if (item.def()->type & Artifact) {
        gain = item.def()->properties[ItemLevel] * 20;
    } else if (item.def()->type & Tool) {
        gain = item.def()->properties[ItemLevel] * 15;
    } else {
        gain = item.def()->properties[ItemLevel] * 5;
    }

    gw()->game()->call_hooks(HookCalcLoreGain, [&](Character &) -> HookPayload { return { &gain }; });
    return gain;
}
