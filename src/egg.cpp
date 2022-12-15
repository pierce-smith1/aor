#include "egg.h"
#include "gamewindow.h"

EggItem::EggItem(CharacterId parent1, CharacterId parent2)
    : Item(EGG_CODE)
{
    instance_properties.map[InstanceEggParent1] = parent1;
    instance_properties.map[InstanceEggParent2] = parent2;
    instance_properties.map[InstanceEggFoundActionstamp] = gw()->game().actions_done();
}

EggItem::EggItem()
    : EggItem(NOBODY, NOBODY) { }

void EggItem::check_hatch() {
    for (const Item &item : gw()->game().inventory().items()) {
        if (item.code == EGG_CODE) {
            if (gw()->game().actions_done() - item.instance_properties[InstanceEggFoundActionstamp] > ACTIONS_TO_HATCH) {
                gw()->game().inventory().remove_item(item.id);
                if (gw()->game().add_character(Generators::yokin_name())) {
                    gw()->notify(Discovery, "A new Fennahian was born!");
                }
                gw()->refresh_ui();
            }
        }
    }
}
