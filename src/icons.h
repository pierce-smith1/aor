#pragma once

#include <QIcon>

#include <map>

#include "items.h"

namespace Icons {
    const std::map<ItemDomain, QPixmap> activity_icons();
    const std::map<ItemDomain, QPixmap> active_status_icons();
    const std::map<ItemDomain, QPixmap> inactive_status_icons();
}
