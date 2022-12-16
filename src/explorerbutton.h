#pragma once

#include <QProgressBar>
#include <QGraphicsColorizeEffect>
#include <QPaintEngine>
#include <QGroupBox>
#include <QLabel>

#include <algorithm>

class LKGameWindow;

#include "items.h"
#include "icons.h"
#include "itemslot.h"

class ExplorerButton : public ItemSlot {
    Q_OBJECT

public:
    ExplorerButton(int n);
    CharacterId id();

    Item get_item() override;
    void set_item(const Item &item) override;
    void refresh_pixmap() override;
    ItemDomain type() override;

    static void insert_explorer_buttons();

    int n;

protected:
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    bool do_hovering() override;
    std::optional<TooltipInfo> tooltip_info() override;
    QString character_description();

private:
    QLabel *m_activity_icon;
    QGraphicsColorizeEffect *m_portrait_effect;
};
