#pragma once

#include "timedactivity.h"

class ScanActivity : public TimedActivity {
public:
    ScanActivity(AorInt ms_total, AorInt ms_left);

};
