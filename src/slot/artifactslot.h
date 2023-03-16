#include "externalslot.h"

class ArtifactSlot : public ExternalSlot {
public:
    ArtifactSlot(size_t n);

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void install() override;

protected:
    virtual ItemId &my_item_id() override;
};