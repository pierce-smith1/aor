#pragma once

#include <QDialog>

#include "itemslot.h"

class AboutBox : public QDialog {
public:
    AboutBox();
};

class AboutSlot : public ItemSlot {
public:
    virtual void refresh_pixmap() override;
    virtual std::optional<TooltipInfo> tooltip_info() override;

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
};

