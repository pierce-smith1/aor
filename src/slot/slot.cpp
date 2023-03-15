#include "slot.h"

#include "../items.h"

SlotMessage::SlotMessage(SlotMessageType type, const PayloadVariant &data, Slot *source)
    : PayloadVariant(data), source(source), type(type) {}

QString SlotMessage::to_data_string() const {
    QString string = QString("%1/%2/%3/%4").arg(type).arg((uintptr_t) source);

    if (std::holds_alternative<std::monostate>(*this)) {
        return string.arg(PV_EMPTY).arg("");
    } else if (std::holds_alternative<Item>(*this)) {
        return string.arg(PV_ITEM).arg(std::get<Item>(*this).to_data_string());
    } else if (std::holds_alternative<CharacterId>(*this)) {
        return string.arg(PV_CHARACTERID).arg(std::get<CharacterId>(*this));
    } else {
        bugcheck(NonExhaustivePayloadSerialization);
        return "";
    }
}

SlotMessage SlotMessage::from_data_string(const QString &string) {
    QStringList chunks = string.split("/");

    SlotMessageType type = static_cast<SlotMessageType>(chunks[0].toULongLong());
    Slot *source = reinterpret_cast<Slot *>(chunks[1].toULongLong()); // oh man oh no

    PayloadVariant data;
    switch (chunks[2][0].toLatin1()) {
        case PV_EMPTY: {
            data = std::monostate();
            break;
        }
        case PV_ITEM: {
            data = Item::from_data_string(chunks[3]);
            break;
        }
        case PV_CHARACTERID:
        case PV_ACTIVITYID: {
            data = CharacterId(chunks[3].toULongLong());
            break;
        }
    }

    return SlotMessage(type, data, source);
}

Slot::Slot()
    : Hoverable<QFrame>(gw()->tooltip(), gw()),
      m_opacity_effect(new QGraphicsOpacityEffect(this))
{
    setMinimumSize(QSize(56, 56));
    setMaximumSize(QSize(56, 56));
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setMouseTracking(true);
    setAcceptDrops(true);

    setProperty("slot", true); // So that we can reference these widgets in a stylesheet.

    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_item_layout = layout;

    QLabel *label = new QLabel(this);
    label->setMinimumSize(QSize(48, 48));
    label->setMaximumSize(QSize(48, 48));
    label->setMouseTracking(true);
    m_item_label = label;

    layout->addWidget(label, 0, 0);

    m_opacity_effect->setOpacity(1.0);
    m_item_label->setGraphicsEffect(m_opacity_effect);

    gw()->register_slot(this);
}

void Slot::refresh() { m_item_label->setPixmap(pixmap()); }
QPixmap Slot::pixmap() { return Item::pixmap_of(Item()); }
bool Slot::will_accept_drop(const SlotMessage &) { return false; }
bool Slot::is_draggable() { return false; }
void Slot::accept_message(const SlotMessage &) {}
void Slot::on_left_click(QMouseEvent *) {}
void Slot::on_right_click(QMouseEvent *) {}
PayloadVariant Slot::user_drop_data() { return std::monostate(); }
void Slot::install() { /* TODO: bugcheck(UnimplementedSlotInstall, typeid(this).name()); */ qWarning("unimplented slot install"); }

void Slot::make_wide() {
    setMinimumSize(QSize(56, 56));
    setMaximumSize(QSize(99999, 56));
}

void Slot::make_tall() {
    setMinimumSize(QSize(56, 56));
    setMaximumSize(QSize(56, 99999));
}

void Slot::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && is_draggable()) {
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;
        SlotMessage message = SlotMessage(SlotUserDrop, user_drop_data(), this);

        data->setText(message.to_data_string());
        drag->setMimeData(data);

        if (std::holds_alternative<Item>(message)) {
            drag->setPixmap(Item::pixmap_of(std::get<Item>(message)));
        }

        drag->exec();
    }
}

void Slot::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        on_right_click(event);
    }

    if (event->button() == Qt::LeftButton) {
        on_left_click(event);
    }

    gw()->refresh_ui();
}

void Slot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data = event->mimeData();
    SlotMessage payload = SlotMessage::from_data_string(data->text());

    if (!will_accept_drop(payload)) {
        return;
    }

    event->acceptProposedAction();
}

void Slot::dropEvent(QDropEvent *event) {
    const QMimeData *data = event->mimeData();
    SlotMessage message = SlotMessage::from_data_string(data->text());

    accept_message(message);

    gw()->refresh_ui();
}
