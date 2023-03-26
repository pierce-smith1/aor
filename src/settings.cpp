#include "settings.h"

void Settings::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, multiwindow_on);
    Serialize::serialize(dev, sounds_on);
}
