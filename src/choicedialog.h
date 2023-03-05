#pragma once

#include <QDialog>

#include "itemslot.h"

class Item;

class ChoiceDialog : public QDialog {
public:
    ChoiceDialog(const WeightedVector<Item> &items);
};

class ChoiceSlot : public ItemSlot {
public:
    ChoiceSlot(ItemCode code, ChoiceDialog *parent, size_t n);

    Item get_item() override;
    void set_item(const Item &item) override;
    void refresh_pixmap() override;

    std::optional<Item> tooltip_item() override;

    size_t n;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

private:
    ItemCode m_item_code;
    ChoiceDialog *m_dialog;
};
