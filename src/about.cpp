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

    slot->refresh();
}
