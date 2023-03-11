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
    Encyclopedia();

    void refresh();

private:
    int m_total_items = 0;
    Ui::Encyclopedia m_widget;
};

class EncyclopediaSlot : public ItemSlot {};
