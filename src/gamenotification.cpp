#include "gamenotification.h"

GameNotification::GameNotification(NotificationType type, const QString &message)
    : QListWidgetItem(icon_for(type), message + " : " + QTime::currentTime().toString()) { }

QIcon GameNotification::icon_for(NotificationType type) {
    return QIcon(":/assets/img/icons/leaf.png");
}
