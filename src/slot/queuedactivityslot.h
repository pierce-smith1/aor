#pragma once

#include "itemslot.h"

class QueuedActivitySlot : public Slot {
public:
    QueuedActivitySlot(size_t n);

    virtual bool do_hovering() override;
    std::optional<TooltipInfo> tooltip_info() override;

    virtual QPixmap pixmap() override;
    virtual void on_right_click(QMouseEvent *event) override;
    virtual void install() override;

private:
    CharacterActivity *activity();

    size_t m_n;
};
