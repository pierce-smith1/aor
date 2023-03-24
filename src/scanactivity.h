#pragma once

#include "timedactivity.h"

class ScanActivity : public TimedActivity {
public:
    ScanActivity(AorInt ms_total);
    ScanActivity(AorInt ms_total, AorInt ms_left);

    virtual void complete() override;
    virtual void update_ui() override;
    virtual ItemDomain type() override;
};
