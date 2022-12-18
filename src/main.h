#pragma once

#include <cstring>

#include <QtGui>
#include <QApplication>
#include <QLabel>
#include <QMessageBox>

#include "itemslot.h"
#include "gamewindow.h"
#include "cheatconsole.h"
#include "encyclopedia.h"

const static std::string GAME_VERSION = "alpha 1.0";

void new_game_prompt();
