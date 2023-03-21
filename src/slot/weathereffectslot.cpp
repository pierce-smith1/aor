#include "weathereffectslot.h"

WeatherEffectSlot::WeatherEffectSlot(size_t n)
    : m_n(n)
{
    make_tall();
}

bool WeatherEffectSlot::is_draggable() {
    return false;
}

void WeatherEffectSlot::install() {
    gw()->window().weather_box->layout()->addWidget(this);
}

Item WeatherEffectSlot::my_item() {
    LocationDefinition here = LocationDefinition::get_def(gw()->game()->current_location_id());
    return Item(here.properties[(ItemProperty) (WeatherEffect1 + m_n)]);
}
