#include "artifactslot.h"

ArtifactSlot::ArtifactSlot(size_t n)
    : ExternalSlot(n)
{
    make_tall();
}

bool ArtifactSlot::will_accept_drop(const SlotMessage &message) {
    if (!ItemSlot::will_accept_drop(message)) {
        return false;
    }

    Item item = get_item(message);
    return Item::def_of(item)->type & Artifact;
}

void ArtifactSlot::on_right_click(QMouseEvent *event) {
    if (my_item().def()->type & Signature) {
        return;
    }

    ExternalSlot::on_right_click(event);
}

bool ArtifactSlot::is_draggable() {
    return !(my_item().def()->type & Signature);
}

void ArtifactSlot::accept_message(const SlotMessage &message) {
    if (message.type == SlotSetItem && get_item(message).def()->type & Signature) {
        bool user_accepts_drop = QMessageBox::question(gw(),
            "Aegis of Rhodon",
            QString(
                "Are you sure you want to give this %2 to %1?"
                "<b>Signature artifacts cannot be unequipped,</b> even if their holder dies!"
            ).arg(gw()->selected_char().name()).arg(get_item(message).def()->display_name)
        ) == QMessageBox::Yes;
        if (!user_accepts_drop) {
            return;
        }
    }

    ExternalSlot::accept_message(message);
    CharacterActivity::refresh_ui_bars(gw()->selected_char());
}

void ArtifactSlot::install() {
    gw()->window().artifact_layout->addWidget(this);
}

ItemId &ArtifactSlot::my_item_id() {
    return gw()->selected_char().external_items()[Artifact][m_n];
}
