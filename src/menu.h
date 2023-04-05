// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QMenuBar>

class LKGameWindow;

class MenuBar : public QMenuBar {
public:
    MenuBar(LKGameWindow *parent);
};
