// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "studyslot.h"
#include "../studyactivity.h"

StudySlot::StudySlot(ItemDomain type, size_t n)
    : ExternalSlot(n), m_type(type) {}

bool StudySlot::will_accept_drop(const SlotMessage &message) {
    if (!ExternalSlot::will_accept_drop(message)) {
        return false;
    }

    return (get_item(message).def()->type & m_type) && (my_item_id() == EMPTY_ID);
}

void StudySlot::accept_message(const SlotMessage &message) {
    if (message.type == SlotForgetItem) {
        return;
    }

    ExternalSlot::accept_message(message);

    if (message.type == SlotSetItem) {
        TimedActivity activity(60000, 60000, Study, { my_item_id() });
        gw()->game()->inventory().get_item_ref(my_item_id()).owning_action = activity.id;
        gw()->game()->register_activity(activity).start();
    }
}

bool StudySlot::is_draggable() {
    return false;
}

void StudySlot::install() {
    switch (m_type) {
        case Consumable: {
            gw()->window().study_consumable_slots->layout()->addWidget(this);
            gw()->window().study_consumable_slots->layout()->setAlignment(this, Qt::AlignCenter);
            break;
        } case Tool: {
            gw()->window().study_tool_slots->layout()->addWidget(this);
            gw()->window().study_tool_slots->layout()->setAlignment(this, Qt::AlignCenter);
            break;
        } case Artifact: {
            gw()->window().study_artifact_slots->layout()->addWidget(this);
            gw()->window().study_artifact_slots->layout()->setAlignment(this, Qt::AlignCenter);
            break;
        } default: {}
    }
}

ItemId &StudySlot::my_item_id() {
    return gw()->game()->studied_items()[m_type][m_n];
}
