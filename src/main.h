// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <cstring>

#include <QtGui>
#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "slot/itemslot.h"
#include "gamewindow.h"
#include "cheatconsole.h"
#include "encyclopedia.h"

const static AorUInt AOR_MAJOR_VERSION = 3;
const static AorUInt AOR_MINOR_VERSION = 0;
const static AorUInt AOR_PATCH_VERSION = 0;

void new_game_prompt();
void welcome_message();
