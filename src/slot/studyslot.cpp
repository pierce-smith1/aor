#include "studyslot.h"

StudySlot::StudySlot(ItemDomain type, size_t n)
    : ExternalSlot(n), m_type(type) {}

bool StudySlot::will_accept_drop(const SlotMessage &message) {
    if (!ExternalSlot::will_accept_drop(message)) {
        return false;
    }

    return get_item(message).def()->type & m_type;
}

bool StudySlot::is_draggable() {
    return false;
}

void StudySlot::install() {
    switch (m_type) {
        case Consumable: {
            gw()->window().study_consumable_slots->layout()->addWidget(this);
            break;
        } case Tool: {
            gw()->window().study_tool_slots->layout()->addWidget(this);
            break;
        } case Artifact: {
            gw()->window().study_artifact_slots->layout()->addWidget(this);
            break;
        } default: {}
    }
}
