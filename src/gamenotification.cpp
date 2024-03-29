// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "gamenotification.h"

GameNotification::GameNotification(NotificationType type, const QString &message)
    : QListWidgetItem(icon_for(type), message + " \t " + QTime::currentTime().toString()) { }

QIcon GameNotification::icon_for(NotificationType type) {
    switch (type) {
        case Discovery: {
            return QIcon(":/assets/img/icons/sun.png");
        }
        case Warning: {
            return QIcon(":/assets/img/icons/warning.png");
        }
        case ActionComplete: {
            return QIcon(":/assets/img/icons/check.png");
        }
        default: {
            return QIcon(":/assets/img/icons/leaf.png");
        }
    }
}
