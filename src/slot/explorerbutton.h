#pragma once

#include <QProgressBar>
#include <QGraphicsColorizeEffect>
#include <QPaintEngine>
#include <QGroupBox>
#include <QLabel>

#include <algorithm>

class LKGameWindow;

#include "../items.h"
#include "../icons.h"
#include "itemslot.h"

const static int EXPLORER_COLS = 3;

class ExplorerButton : public ItemSlot {};
