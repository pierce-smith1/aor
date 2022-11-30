#pragma once

#include <QProgressBar>
#include <QGraphicsColorizeEffect>
#include <QGroupBox>
#include <QLabel>

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
    std::map<ItemDomain, QLabel*> m_tool_status_labels;
    std::array<QLabel*, ARTIFACT_SLOTS> m_artifact_status_labels {};
    QGraphicsColorizeEffect *m_portrait_effect;
};
