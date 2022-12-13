#pragma once

#include <utility>
#include <optional>

#include <QFrame>
#include <QLabel>
#include <QString>
#include <QWidget>
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsOpacityEffect>

#include "gamewindow.h"
#include "tooltip.h"

const static int INVALID_COORD = -1;

class ItemSlot : public Hoverable<QFrame> {
    Q_OBJECT

public:
    ItemSlot();
    ItemSlot(int y, int x);

    virtual Item get_item();
    virtual void set_item(const Item &item);
    virtual ItemDomain type();
    virtual void refresh_pixmap();
    void drop_external_item();

    static void insert_inventory_slots();
    static void insert_inventory_slot(unsigned y, unsigned x);
    static QString make_internal_name(const QString &base, int y, int x);

    int y;
    int x;

protected:
    virtual bool do_hovering() override;
    virtual std::optional<Item> tooltip_item() override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    QLabel *m_item_label;
    QLayout *m_item_layout;

private:
    QGraphicsOpacityEffect *m_opacity_effect;
};
