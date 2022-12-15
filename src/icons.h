#pragma once

#include <QIcon>

#include <map>

#include "items.h"

namespace Icons {
    const std::map<ItemDomain, QPixmap> &activity_icons();
    const std::map<ItemProperty, QPixmap> &resource_icons();
}
