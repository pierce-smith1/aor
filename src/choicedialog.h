#pragma once

#include <QDialog>

#include "itemslot.h"

class Item;

class ChoiceDialog : public QDialog {
public:
    ChoiceDialog(const WeightedVector<Item> &items);
};

class ChoiceSlot : public ItemSlot {};
