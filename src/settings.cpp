#include "settings.h"

void Settings::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, multiwindow_on);
    Serialize::serialize(dev, sounds_on);
}

void Settings::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, multiwindow_on);
    Serialize::deserialize(dev, sounds_on);
}
