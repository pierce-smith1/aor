#pragma once

#include <QDialog>

#include "slot/choiceslot.h"

class Item;

class ChoiceDialog : public QDialog {
public:
    ChoiceDialog(const WeightedVector<Item> &items);
};
