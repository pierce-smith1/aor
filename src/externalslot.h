#pragma once

#include "itemslot.h"

const static int PRAYER_KEY_SLOT_Y = 0;
const static int PRAYER_KEY_SLOT_X = 1;

class ExternalSlot : public ItemSlot {
    Q_OBJECT

public:
    ExternalSlot(LKGameWindow *game, ItemDomain item_slot_type, int n);

    virtual Item get_item() override;
    virtual void set_item(const Item &item) override;
    virtual ItemDomain type() override;
    virtual void refresh_pixmap() override;
    virtual ItemId held_item_id();

    static void insert_external_slots(LKGameWindow &window);

    ItemDomain item_slot_type;
    int n;

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
};

class ToolSlot : public ExternalSlot {
    Q_OBJECT

public:
    ToolSlot(LKGameWindow *game, ItemDomain tool_slot_type);

    void set_item(const Item &item) override;
    void refresh_pixmap() override;
    ItemId held_item_id() override;

    ItemDomain get_tool_slot_type();

    static void insert_tool_slots(LKGameWindow &window);

private:
    ItemDomain m_tool_slot_type;
};

class PortraitSlot : public ExternalSlot {
    Q_OBJECT

public:
    PortraitSlot(LKGameWindow *game);

    Item get_item() override;
    void set_item(const Item &item) override;
    void refresh_pixmap() override;

    static void insert_portrait_slot(LKGameWindow &window);

protected:
    void enterEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};
