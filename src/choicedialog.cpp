#include "choicedialog.h"
#include "items.h"
#include "gamewindow.h"

ChoiceDialog::ChoiceDialog(const WeightedVector<Item> &items) {
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel(this);
    label->setText("<i>Make your choice...</i>");
    label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(label);

    QWidget *slot_area = new QWidget();
    QHBoxLayout *slot_layout = new QHBoxLayout();

    for (size_t n = 0; n < items.size(); n++) {
        const Item &item = items[n].first;
        slot_layout->addWidget(new ChoiceSlot(item.code, this, n));
    }
    slot_area->setLayout(slot_layout);
    layout->addWidget(slot_area);

    setLayout(layout);

    setMaximumSize(QSize(9999999, 56));
    setMinimumSize(QSize(0, 120));
}

ChoiceSlot::ChoiceSlot(ItemCode code, ChoiceDialog *parent, size_t n)
    : n(n), m_item_code(code), m_dialog(parent)
{
    setParent(parent);
    refresh_pixmap();
}

void ChoiceSlot::set_item(const Item &item) {
    m_item_code = item.code;
}

Item ChoiceSlot::get_item() {
    return Item(m_item_code);
}

void ChoiceSlot::refresh_pixmap() {
    m_item_label->setPixmap(Item::pixmap_of(m_item_code));
}

std::optional<Item> ChoiceSlot::tooltip_item() {
    return std::optional<Item>(Item(m_item_code));
}

void ChoiceSlot::mousePressEvent(QMouseEvent *) {
    m_dialog->done(n);
}

void ChoiceSlot::dragEnterEvent(QDragEnterEvent *) {}
