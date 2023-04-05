// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QListWidgetItem>
#include <QTime>
#include <QString>

#include "types.h"

enum NotificationType : AorInt {
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
