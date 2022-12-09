#pragma once

#include <QGraphicsColorizeEffect>

#include "../ui_encyclopedia.h"
#include "gamewindow.h"
#include "itemslot.h"

const static int ENCYCLOPEDIA_GROUP_ROWS = 3;
const static int ENCYCLOPEDIA_GROUP_COLS = 10;

class Encyclopedia : public QWidget {
    Q_OBJECT

public:
    Encyclopedia(LKGameWindow *game_window);

private:
    LKGameWindow *m_game_window;
    Ui::Encyclopedia m_widget;
};

class EncyclopediaSlot : public ItemSlot {
public:
    EncyclopediaSlot(LKGameWindow *game_window, int y, int x, ItemCode item_group);

    bool undiscovered();

    Item get_item() override;
    void set_item(const Item &item) override;
    void refresh_pixmap() override;

protected:
    void enterEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    int m_y;
    int m_x;
    ItemCode m_item_group;
};
