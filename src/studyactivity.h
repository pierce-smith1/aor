#pragma once

#include "timedactivity.h"

class StudyActivity {
public:
    static void complete(const TimedActivity &activity);
    static void update_ui(const TimedActivity &activity);
    static AorInt lore_to_gain(const TimedActivity &activity);
};
