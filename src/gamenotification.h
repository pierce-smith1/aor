#pragma once

#include <QListWidgetItem>
#include <QTime>
#include <QString>

enum NotificationType : int {
    Discovery = 2000,
    Warning,
    ActionComplete
};

class GameNotification : public QListWidgetItem {
public:
    GameNotification(NotificationType type, const QString &message);

private:
    static QIcon icon_for(NotificationType type);
};
