#include "externalslot.h"

ExternalSlot::ExternalSlot(size_t n)
    : m_n(n) {}

ExternalItemIds &ExternalSlot::external_ids() {
    return gw()->selected_char().external_items();
}
