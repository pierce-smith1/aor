// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "smithingresultslot.h"

void SmithingResultSlot::install() {
    gw()->window().smith_button_layout->addWidget(this);
}

Item SmithingResultSlot::my_item() {
    return Item(gw()->selected_char().smithing_result());
}
