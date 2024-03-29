// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "serialize.h"

struct Settings : public Serializable {
    bool multiwindow_on = false;
    bool sounds_on = true;

    void serialize(QIODevice *dev) const;
    void deserialize(QIODevice *dev);
};
