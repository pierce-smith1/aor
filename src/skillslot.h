#pragma once

#include "itemslot.h"

class SkillSlot : public ItemSlot {
public:
    virtual Item get_item() override;
    virtual void set_item(const Item &item) override;
    virtual void refresh_pixmap() override;

    static void insert_skill_slots();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
};
