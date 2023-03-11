#include "itemslot.h"
#include "items.h"

DropPayload::DropPayload(const PayloadVariant &data, ItemSlot *source)
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
    ItemSlot *source = reinterpret_cast<ItemSlot *>(chunks[0].toULongLong());

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

ItemSlot::ItemSlot()
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

void ItemSlot::refresh() { m_item_label->setPixmap(pixmap()); }
QPixmap ItemSlot::pixmap() { return Item::pixmap_of(Item()); }
bool ItemSlot::will_accept_drop(const DropPayload &) { return false; }
bool ItemSlot::is_draggable() { return false; }
void ItemSlot::accept_drop(const DropPayload &) {}
void ItemSlot::after_dropped_elsewhere(const DropPayload &) {}
void ItemSlot::on_left_click(QMouseEvent *) {}
void ItemSlot::on_right_click(QMouseEvent *) {}
DropPayload ItemSlot::get_payload() { return DropPayload(std::monostate(), this); }
void ItemSlot::install() { /* TODO: bugcheck(UnimplementedSlotInstall, typeid(this).name()); */ qWarning("unimplented slot install"); }

void ItemSlot::mousePressEvent(QMouseEvent *event) {
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

void ItemSlot::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        on_right_click(event);
    }

    if (event->button() == Qt::LeftButton) {
        on_left_click(event);
    }
}

void ItemSlot::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *data = event->mimeData();
    DropPayload payload = DropPayload::from_string(data->text());

    if (!will_accept_drop(payload)) {
        return;
    }

    event->acceptProposedAction();
}

void ItemSlot::dropEvent(QDropEvent *event) {
    const QMimeData *data = event->mimeData();
    DropPayload payload = DropPayload::from_string(data->text());

    accept_drop(payload);

    gw()->refresh_ui();
}
