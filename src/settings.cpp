// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "settings.h"

void Settings::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, multiwindow_on);
    Serialize::serialize(dev, sounds_on);
}

void Settings::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, multiwindow_on);
    Serialize::deserialize(dev, sounds_on);
}
