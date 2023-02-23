#include "about.h"
#include "main.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>

AboutBox::AboutBox() {
    setLayout(new QVBoxLayout());

    AboutSlot *slot = new AboutSlot;
    layout()->addWidget(slot);
    layout()->setAlignment(slot, Qt::AlignCenter);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    layout()->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    slot->refresh_pixmap();
}

void AboutSlot::refresh_pixmap() {
    m_item_label->setPixmap(Item::pixmap_of("welchian_rune"));
}

std::optional<TooltipInfo> AboutSlot::tooltip_info() {
    return std::optional<TooltipInfo>({
        "<b>Aegis of Rhodon</b>",
        "version " + GAME_VERSION,
        "<i>Be still, Rhodon.</i><br>"
        "<i>https://doughbyte.com</i>",
        Item::pixmap_of("welchian_rune"),
        {},
        std::optional<QColor>()
    });
}

void AboutSlot::mousePressEvent(QMouseEvent *) {};
void AboutSlot::dragEnterEvent(QDragEnterEvent *) {};
void AboutSlot::dropEvent(QDropEvent *) {};
