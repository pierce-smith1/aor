// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QGraphicsColorizeEffect>

#include "../ui_encyclopedia.h"
#include "gamewindow.h"
#include "slot/encyclopediaslot.h"

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
