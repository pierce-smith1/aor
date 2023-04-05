// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QIcon>

#include <map>

#include "items.h"

namespace Icons {
    const std::map<ItemDomain, QPixmap> &activity_icons();
    const std::map<ItemDomain, QPixmap> &activity_icons_big();
    const std::map<ItemDomain, QPixmap> &activity_portraits();
    const std::map<ItemDomain, QPixmap> &explorer_button_icons();
    const std::map<ItemProperty, QPixmap> &resource_icons();
}
