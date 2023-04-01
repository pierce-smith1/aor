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

const static AorUInt MAJOR_VERSION = 2;
const static AorUInt MINOR_VERSION = 0;
const static AorUInt PATCH_VERSION = 1;

void new_game_prompt();
void welcome_message();
