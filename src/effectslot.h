#pragma once

#include "itemslot.h"

class EffectSlot : public ItemSlot {
    Q_OBJECT

public:
    EffectSlot(LKGameWindow *game, int n);

    Item get_item() override;
    void set_item(const Item &item) override;
    ItemDomain type() override;
    void refresh_pixmap() override;

    static void insert_effect_slots(LKGameWindow &window);
    static QString make_internal_name(const QString &base, int n);

    int n;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

private:
    ItemCode m_effect_code;
};
