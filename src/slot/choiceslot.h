#pragma once

#include "slot.h"

class ChoiceDialog;

class ChoiceSlot : public Slot {
public:
    ChoiceSlot(ItemCode code, ChoiceDialog *parent, size_t n);

    virtual bool do_hovering() override;
    virtual std::optional<Item> tooltip_item() override;

    virtual QPixmap pixmap() override;
    virtual void on_left_click(QMouseEvent *event) override;

private:
    ItemCode m_code;
    ChoiceDialog *m_dialog;
    size_t m_n;
};
