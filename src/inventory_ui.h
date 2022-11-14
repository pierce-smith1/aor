#pragma once

#include <utility>

#include <QFrame>
#include <QLabel>
#include <QString>

#include "gamewindow.h"

namespace InventoryUi {
    void insert_inventory_slots(LKGameWindow &window);
    void insert_inventory_slot(LKGameWindow &window, unsigned y, unsigned x);
    QFrame *make_inventory_slot(LKGameWindow &window, unsigned y, unsigned x);
    QLabel *get_inventory_label(LKGameWindow &window, unsigned y, unsigned x);
    QString make_internal_name(const std::string &base, unsigned y, unsigned x);

    std::pair<int, int> get_yx_coords_of_label(QObject *label);
    ItemId get_item_in(LKGameWindow &window, QObject *label);

    bool are_yx_coords_oob(int y, int x);
    size_t inventory_index(int y, int x);
}
