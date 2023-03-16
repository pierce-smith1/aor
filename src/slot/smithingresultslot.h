#include "itemslot.h"

class SmithingResultSlot : public ItemSlot {
public:
    SmithingResultSlot() = default;

    virtual void install() override;

protected:
    virtual Item my_item() override;
};
