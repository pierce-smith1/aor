#pragma once

#include <QSoundEffect>

#include <map>

#include "items.h"

namespace Sounds {
    std::map<ItemDomain, QSoundEffect *> activity_sounds();
    QSoundEffect *grab_sound();
    QSoundEffect *drop_sound();
    QSoundEffect *hover_sound();
    QSoundEffect *get_sound(const QString &name);
};
