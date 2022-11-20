#pragma once

#include "itemslot.h"

class ExternalSlot : public ItemSlot {
    Q_OBJECT

public:
    ExternalSlot(LKGameWindow *game, SlotType type, int n);

    virtual Item get_item() override;
    virtual void set_item(const Item &item) override;
    virtual SlotType get_type() override;
    virtual void refresh_pixmap() override;

    static void insert_external_slots(LKGameWindow &window);

    SlotType type;
    int n;

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

private:
    ItemId held_item_id;
};

class ToolSlot : public ExternalSlot {
    Q_OBJECT

public:
    ToolSlot(LKGameWindow *game);

    static void insert_tool_slot(LKGameWindow &window);
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
    void dropEvent(QDropEvent *event) override;
};
