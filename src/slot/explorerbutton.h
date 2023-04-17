// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

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
    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void accept_message(const SlotMessage &message) override;
    virtual bool is_draggable() override;
    virtual void on_left_click(QMouseEvent *event) override;
    virtual PayloadVariant user_drop_data() override;
    virtual void install() override;

protected:
    QLabel *m_activity_icon;
    QLabel *m_couple_status_icon;
    QWidget *m_status_icons_widget;
    QLabel *m_tool_status_icon;
    QLabel *m_artifact_status_icon;
    QGraphicsColorizeEffect *m_portrait_effect;

private:
    Character &character();
    QString character_description();

    size_t m_n;
};
