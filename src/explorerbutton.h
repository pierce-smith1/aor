#pragma once

#include <QProgressBar>
#include <QGraphicsColorizeEffect>
#include <QGroupBox>

class LKGameWindow;

#include "items.h"
#include "icons.h"

class ExplorerButton : public QGroupBox {
    Q_OBJECT

public:
    ExplorerButton(QWidget *parent, LKGameWindow *game_window, CharacterId id);

    CharacterId id();
    void refresh();

protected:
    void mousePressEvent(QMouseEvent *) override;

private:
    LKGameWindow *m_game_window;
    CharacterId m_id;
    QLabel *m_name;
    QLabel *m_portrait;
    QLabel *m_activity_icon;
    QProgressBar *m_activity_bar;
    QProgressBar *m_morale_bar;
    QProgressBar *m_energy_bar;
    QGraphicsColorizeEffect *m_portrait_effect;
};
