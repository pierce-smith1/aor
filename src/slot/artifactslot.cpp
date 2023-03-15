#include "artifactslot.h"

ArtifactSlot::ArtifactSlot(size_t n)
    : ExternalSlot(n)
{
    make_tall();
}

bool ArtifactSlot::will_accept_drop(const SlotMessage &message) {
    Item item = get_item(message);
    return Item::def_of(item)->type & Artifact;
}

void ArtifactSlot::install() {
    gw()->window().artifact_layout->addWidget(this);
}

ItemId &ArtifactSlot::my_item_id() {
    return gw()->selected_char().external_items()[Artifact][m_n];
}


