#include "slot.h"
#include "items.h"

DropPayload::DropPayload(const PayloadVariant &data, Slot *source)
    : PayloadVariant(data), source(source) {}

QString DropPayload::to_string() const {
    QString string = QString("%1;%2;%3").arg((uintptr_t) source);

    if (std::holds_alternative<std::monostate>(*this)) {
        return string.arg(PV_EMPTY).arg("");
    } else if (std::holds_alternative<ItemId>(*this)) {
        return string.arg(PV_ITEM).arg(std::get<ItemId>(*this));
    } else if (std::holds_alternative<CharacterId>(*this)) {
        return string.arg(PV_CHARACTER).arg(std::get<CharacterId>(*this));
    } else {
        bugcheck(NonExhaustivePayloadSerialization);
        return "";
    }
}

DropPayload DropPayload::from_string(const QString &string) {
    QStringList chunks = string.split(";");

    // oh man oh no
    Slot *source = reinterpret_cast<Slot *>(chunks[0].toULongLong());

    PayloadVariant data;
    switch (chunks[1][0].toLatin1()) {
        case PV_EMPTY: {
            data = std::monostate();
            break;
        }
        case PV_ITEM:
        case PV_CHARACTER:
        case PV_ACTIVITY: {
            data = ItemId(chunks[2].toULongLong());
            break;
        }
    }

    return DropPayload(data, source);
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
bool Slot::will_accept_drop(const DropPayload &) { return false; }
bool Slot::is_draggable() { return false; }
void Slot::accept_drop(const DropPayload &) {}
void Slot::after_dropped_elsewhere(const DropPayload &) {}
void Slot::on_left_click(QMouseEvent *) {}
void Slot::on_right_click(QMouseEvent *) {}
DropPayload Slot::get_payload() { return DropPayload(std::monostate(), this); }
void Slot::install() { /* TODO: bugcheck(UnimplementedSlotInstall, typeid(this).name()); */ qWarning("unimplented slot install"); }
ItemDomain Slot::type() { return None; }

void Slot::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && is_draggable()) {
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;
        DropPayload payload = get_payload();

        data->setText(payload.to_string());
        drag->setMimeData(data);

        if (std::holds_alternative<ItemId>(payload)) {
            drag->setPixmap(Item::pixmap_of(gw()->game().inventory().get_item(std::get<ItemId>(payload))));
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
    DropPayload payload = DropPayload::from_string(data->text());

    if (!will_accept_drop(payload)) {
        return;
    }

    event->acceptProposedAction();
}

void Slot::dropEvent(QDropEvent *event) {
    const QMimeData *data = event->mimeData();
    DropPayload payload = DropPayload::from_string(data->text());

    accept_drop(payload);

    gw()->refresh_ui();
}
