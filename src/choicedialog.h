// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QDialog>

#include "slot/choiceslot.h"

class Item;

class ChoiceDialog : public QDialog {
public:
    ChoiceDialog(const WeightedVector<Item> &items);
};
