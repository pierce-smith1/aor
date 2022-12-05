#pragma once

#include <utility>

#include <QFrame>
#include <QLabel>
#include <QString>
#include <QWidget>
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsOpacityEffect>

#include "gamewindow.h"

const static int INVALID_COORD = -1;

class ItemSlot : public QFrame {
    Q_OBJECT

public:
    ItemSlot(LKGameWindow *game);
    ItemSlot(LKGameWindow *game, int y, int x);

    virtual Item get_item();
    virtual void set_item(const Item &item);
    virtual ItemDomain type();
    virtual void refresh_pixmap();
    void drop_external_item();

    static void insert_inventory_slots(LKGameWindow &window);
    static void insert_inventory_slot(LKGameWindow &window, unsigned y, unsigned x);
    static QString make_internal_name(const QString &base, int y, int x);

    int y;
    int x;

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    QLabel *m_item_label;
    QLayout *m_item_layout;
    LKGameWindow *m_game_window;

private:
    QGraphicsOpacityEffect m_opacity_effect;
};
