#include "gamenotification.h"

GameNotification::GameNotification(NotificationType type, const std::string &message)
    : QListWidgetItem(icon_for(type), QString::fromStdString(message) + " : " + QTime::currentTime().toString()) { }

QIcon GameNotification::icon_for(NotificationType type) {
    return QIcon(":/assets/img/icons/leaf.png");
}
