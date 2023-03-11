#include "externalslot.h"
#include "items.h"
#include "foreigntradeslot.h"
#include "smithingresultslot.h"
#include "die.h"
#include "icons.h"

ExternalSlot::ExternalSlot(size_t n)
    : m_n(n) {}

ExternalItemIds &ExternalSlot::external_ids() {
    return gw()->selected_char().external_items();
}
