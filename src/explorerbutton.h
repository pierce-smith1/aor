#pragma once

#include <QProgressBar>
#include <QGraphicsColorizeEffect>
#include <QGroupBox>
#include <QLabel>

class LKGameWindow;

#include "items.h"
#include "icons.h"
#include "tooltip.h"

class ExplorerButton : public Hoverable<QGroupBox> {
    Q_OBJECT

public:
    ExplorerButton(QWidget *parent, CharacterId id);

    CharacterId id();
    void refresh();

protected:
    void mousePressEvent(QMouseEvent *) override;

    bool do_hovering() override;
    std::optional<TooltipInfo> tooltip_info() override;

private:
    CharacterId m_id;
    QLabel *m_name;
    QLabel *m_portrait;
    QLabel *m_activity_icon;
    QGraphicsColorizeEffect *m_portrait_effect;
};
