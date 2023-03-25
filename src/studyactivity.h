#pragma once

#include "timedactivity.h"

class StudyActivity : public TimedActivity {
public:
    StudyActivity(AorInt ms_total, ItemId item_id);

    void complete() override;
    void update_ui() override;
    ItemDomain type() override;

protected:
    AorInt lore_to_gain();

    ItemId m_item_id;
};
