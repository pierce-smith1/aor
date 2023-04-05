// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "timedactivity.h"

class StudyActivity {
public:
    static void complete(const TimedActivity &activity);
    static void update_ui(const TimedActivity &activity);
    static AorInt lore_to_gain(const TimedActivity &activity);
};
