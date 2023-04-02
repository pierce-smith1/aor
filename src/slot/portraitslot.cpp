#include "portraitslot.h"
#include "../icons.h"

PortraitSlot::PortraitSlot()
    : m_container(new QWidget(parentWidget()))
{
    m_container->setLayout(new QGridLayout());
    m_container->layout()->addWidget(this);
    m_container->layout()->setAlignment(this, Qt::AlignCenter);

    setMinimumSize(160, 230);
    setMaximumSize(160, 230);
    m_item_label->setMinimumSize(156, 226);
    m_item_label->setMaximumSize(156, 226);
}

void PortraitSlot::refresh() {
    Slot::refresh();

    QString stylesheet = "border-radius: 5px;";

    if (gw()->selected_char().activity().explorer_subtype() == Coupling) {
        stylesheet += (QString("background: qlineargradient(x1: 0.385, y1: 0.48, x2: 0.3875, y2: 0.482,")
            + "stop: 0 " + Colors::blend(gw()->selected_char().heritage()).name() + ","
            + "stop: 1 " + Colors::blend(gw()->game()->character(gw()->selected_char().partner()).heritage()).name() + ");"
        );
    } else {
        stylesheet += QString("background-color: %1;")
            .arg(Colors::blend(gw()->selected_char().heritage()).name());
    }

    setStyleSheet(stylesheet);
}

bool PortraitSlot::will_accept_drop(const SlotMessage &message) {
    if (!std::holds_alternative<Item>(message)) {
        return false;
    }

    Item item = std::get<Item>(message);
    return Item::def_of(item)->type & (Consumable | Material);
}

void PortraitSlot::accept_message(const SlotMessage &message) {
    switch (message.type) {
        case SlotUserDrop: {
            Item item = std::get<Item>(message);
            if (item.def()->type & Consumable) {
                gw()->selected_char().queue_activity(Eating, { item.id });
            } else if (item.def()->type & Material) {
                gw()->selected_char().queue_activity(Defiling, { item.id });
            }
            break;
        } default: {}
    }
}

QPixmap PortraitSlot::pixmap() {
    if (gw()->selected_char().dead()) {
        return QPixmap(":/assets/img/lk/dead.png");
    } else {
        return Icons::activity_portraits().at(gw()->selected_char().activity().explorer_subtype());
    }
}

void PortraitSlot::install() {
    QGridLayout *layout = dynamic_cast<QGridLayout *>(gw()->window().player_widget->layout());
    layout->addWidget(m_container, 2, 1);
}
