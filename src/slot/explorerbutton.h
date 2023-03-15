#pragma once

#include <QLabel>
#include <QGraphicsColorizeEffect>

#include "slot.h"

const static int EXPLORER_COLS = 3;

class ExplorerButton : public Slot {
public:
    ExplorerButton(size_t n);

    virtual bool do_hovering() override;
    virtual std::optional<TooltipInfo> tooltip_info() override;

    virtual void refresh() override;
    virtual QPixmap pixmap() override;
    virtual bool is_draggable() override;
    virtual void on_left_click(QMouseEvent *event) override;
    virtual void install() override;

protected:
    QLabel *m_activity_icon;
    QLabel *m_couple_status_icon;
    QGraphicsColorizeEffect *m_portrait_effect;

private:
    Character &character();
    QString character_description();

    size_t m_n;
};
