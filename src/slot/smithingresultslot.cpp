#include "smithingresultslot.h"

void SmithingResultSlot::install() {
    gw()->window().smith_button_layout->addWidget(this);
}

Item SmithingResultSlot::my_item() {
    return Item(gw()->selected_char().smithing_result());
}
