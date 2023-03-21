#pragma once

#include "itemslot.h"

class WeatherEffectSlot : public ItemSlot {
public:
    WeatherEffectSlot(size_t n);

    bool is_draggable() override;
    void install() override;

protected:
    Item my_item() override;

private:
    size_t m_n;
};
