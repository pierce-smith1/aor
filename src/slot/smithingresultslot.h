// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "itemslot.h"

class SmithingResultSlot : public ItemSlot {
public:
    SmithingResultSlot() = default;

    virtual void install() override;

protected:
    virtual Item my_item() override;
};
