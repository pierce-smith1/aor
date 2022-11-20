#pragma once

#include <QListWidgetItem>
#include <QTime>

enum NotificationType : int {
    Discovery = 2000,
    Warning,
    ActionComplete
};

class GameNotification : public QListWidgetItem {
public:
    GameNotification(NotificationType type, const std::string &message);

private:
    static QIcon icon_for(NotificationType type);
};
