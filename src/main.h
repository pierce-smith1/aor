#pragma once

#include <cstring>

#include <QtGui>
#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "itemslot.h"
#include "gamewindow.h"
#include "cheatconsole.h"
#include "encyclopedia.h"

const static QString GAME_VERSION = "1.0.1";

void new_game_prompt();
void welcome_message();
