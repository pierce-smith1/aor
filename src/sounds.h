#pragma once

#include <QSoundEffect>

#include <map>

#include "items.h"

namespace Sounds {
    std::map<ItemDomain, QSoundEffect *> activity_sounds();
    QSoundEffect *get_sound(const QString &name);
};
