#pragma once

#include <QMenuBar>

class LKGameWindow;

class MenuBar : public QMenuBar {
public:
    MenuBar(LKGameWindow *parent);
};
